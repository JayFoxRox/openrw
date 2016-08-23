###### External Project: mman-win32
include(ExternalProject)

SET(MMAN_WIN32_PREFIX_DIR ${PROJECT_SOURCE_DIR}/external/mman-win32)

SET(MMAN_WIN32_SOURCES
	${MMAN_WIN32_PREFIX_DIR}/mman.c)
add_library(mman
	${MMAN_WIN32_SOURCES})

target_include_directories(mman INTERFACE SYSTEM ${MMAN_WIN32_PREFIX_DIR})
