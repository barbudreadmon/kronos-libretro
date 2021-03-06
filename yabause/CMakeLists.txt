project(kronos)

cmake_minimum_required(VERSION 2.8)

#-------------------------------------------------------------------------------
#add local find scripts to CMAKE path
LIST(APPEND CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Utils
    ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Packages
)

option(YAB_FORCE_GLES20 "force to use OpenGLES 2.0" ON)
if (YAB_FORCE_GLES20)
set(GLSystem "OpenGL ES 2.0")
else(YAB_FORCE_GLES20)
set(GLSystem "Desktop OpenGL")
endif(YAB_FORCE_GLES20)

set(YAB_PACKAGE kronos)
set(YAB_VERSION_MAJOR 1)
set(YAB_VERSION_MINOR 2)
set(YAB_VERSION_PATCH 0)
set(YAB_VERSION "${YAB_VERSION_MAJOR}.${YAB_VERSION_MINOR}.${YAB_VERSION_PATCH}")

set(CPACK_SOURCE_GENERATOR TGZ)
set(CPACK_PACKAGE_VERSION_MAJOR ${YAB_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${YAB_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${YAB_VERSION_PATCH})
set(CPACK_PACKAGE_VENDOR "Francois CARON")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "kronos-${YAB_VERSION}")

if (WIN32)
	SET(CPACK_NSIS_INSTALLED_ICON_NAME kronos.exe)
	set(CPACK_NSIS_MENU_LINKS kronos.exe;Kronos)
	set(CPACK_NSIS_URL_INFO_ABOUT "http://www.kronos-emulator.org/")
	set(CPACK_NSIS_COMPRESSOR "/SOLID lzma")
	if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
		set(CPACK_SYSTEM_NAME "win64")
		set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
		set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME}${CPACK_PACKAGE_VERSION} (Win64)")
	endif ()
endif ()

include(CPack)

add_subdirectory(doc)
add_subdirectory(l10n)
add_subdirectory(src)
