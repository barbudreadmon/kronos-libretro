/*  Copyright 2010 Lawrence Sebald

    This file is part of Yabause.

    Yabause is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Yabause is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Yabause; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include "../yabause/src/threads.h"

#include <stdio.h>
#include <pthread.h>
#include <sched.h>

/* Thread handle structure. */
struct thd_s {
    int running;
    pthread_t thd;
    void (*func)(void *);
    void *arg;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

static struct thd_s thread_handle[YAB_NUM_THREADS];
static pthread_key_t hnd_key;
static pthread_once_t hnd_key_once;

static void make_key() {
    pthread_key_create(&hnd_key, NULL);
}

static void *wrapper(void *hnd) {
    struct thd_s *hnds = (struct thd_s *)hnd;

    pthread_mutex_lock(&hnds->mutex);

    /* Set the handle for the thread, and call the actual thread function. */
    pthread_setspecific(hnd_key, hnd);
    hnds->func(hnds->arg);

    pthread_mutex_unlock(&hnds->mutex);

    return NULL;
}

int YabThreadStart(unsigned int id, void (*func)(void *), void *arg) {
    /* Create the key to access the thread handle if we haven't made it yet. */
    pthread_once(&hnd_key_once, make_key);

    /* Make sure we aren't trying to start a thread twice. */
    if(thread_handle[id].running) {
        fprintf(stderr, "YabThreadStart: Thread %u is already started!\n", id);
        return -1;
    }

    /* Create the mutex and condvar for the thread. */
    if(pthread_mutex_init(&thread_handle[id].mutex, NULL)) {
        fprintf(stderr, "YabThreadStart: Error creating mutex\n");
        return -1;
    }

    if(pthread_cond_init(&thread_handle[id].cond, NULL)) {
        fprintf(stderr, "YabThreadStart: Error creating condvar\n");
        pthread_mutex_destroy(&thread_handle[id].mutex);
        return -1;
    }

    thread_handle[id].func = func;
    thread_handle[id].arg = arg;

    /* Create the thread. */
    if(pthread_create(&thread_handle[id].thd, NULL, wrapper,
                      &thread_handle[id])) {
        fprintf(stderr, "YabThreadStart: Couldn't start thread\n");
        pthread_cond_destroy(&thread_handle[id].cond);
        pthread_mutex_destroy(&thread_handle[id].mutex);
        return -1;
    }

    thread_handle[id].running = 1;

    return 0;
}

void YabThreadWait(unsigned int id) {
    /* Make sure the thread is running. */
    if(!thread_handle[id].running)
        return; 

    /* Join the thread to wait for it to finish. */
    pthread_join(thread_handle[id].thd, NULL);

    /* Cleanup... */
    pthread_cond_destroy(&thread_handle[id].cond);
    pthread_mutex_destroy(&thread_handle[id].mutex);
    thread_handle[id].thd = NULL;
    thread_handle[id].func = NULL;

    thread_handle[id].running = 0;
}

void YabThreadYield(void) {
    sched_yield();
}

void YabThreadSleep(void) {
    struct thd_s *thd = (struct thd_s *)pthread_getspecific(hnd_key);

    /* Wait on the condvar... */
    pthread_cond_wait(&thd->cond, &thd->mutex);
}

void YabThreadWake(unsigned int id) {
    if(!thread_handle[id].running)
        return;

    pthread_cond_signal(&thread_handle[id].cond);
}


typedef struct YabMutex_pthread
{
  pthread_mutex_t mutex;
} YabMutex_pthread;

YabMutex * YabThreadCreateMutex(){
    YabMutex_pthread * mtx = (YabMutex_pthread *)malloc(sizeof(YabMutex_pthread));
    pthread_mutex_init( &mtx->mutex,NULL);
    return (YabMutex *)mtx;
}

void YabThreadUSleep( unsigned int stime )
{
	usleep(stime);
}

typedef struct YabBarrier_pthread
{
  pthread_barrier_t barrier;
} YabBarrier_pthread;

YabBarrier * YabThreadCreateBarrier(int nbWorkers){
    YabBarrier_pthread * mtx = (YabBarrier_pthread *)malloc(sizeof(YabBarrier_pthread));
    pthread_barrier_init( &mtx->barrier,NULL, nbWorkers);
    return (YabBarrier *)mtx;
}

void YabThreadSetCurrentThreadAffinityMask(int mask)
{
#if 0    
    int err, syscallres;
    pid_t pid = gettid();

	cpu_set_t my_set;        /* Define your cpu_set bit mask. */
	CPU_ZERO(&my_set);       /* Initialize it all to 0, i.e. no CPUs selected. */
	CPU_SET(mask, &my_set);
	CPU_SET(mask+4, &my_set);
	sched_setaffinity(pid,sizeof(my_set), &my_set);
#endif    
}

void YabThreadBarrierWait(YabBarrier *bar){
    if (bar == NULL) return;
    YabBarrier_pthread * pctx;
    pctx = (YabBarrier_pthread *)bar;
    pthread_barrier_wait(&pctx->barrier);
}
