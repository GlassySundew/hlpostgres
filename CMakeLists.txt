if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    cmake_minimum_required(VERSION 3.13)
    project(hlpostgres LANGUAGES C CXX)

    include(GNUInstallDirs)
    include(FindPkgConfig)

    set(HASHLINK_DIR "${CMAKE_CURRENT_LIST_DIR}/../.." CACHE PATH "Path to the HashLink repository root")

    set(WITH_VM OFF CACHE BOOL "" FORCE)
    set(WITH_FMT OFF CACHE BOOL "" FORCE)
    set(WITH_OPENAL OFF CACHE BOOL "" FORCE)
    set(WITH_SDL OFF CACHE BOOL "" FORCE)
    set(WITH_SQLITE OFF CACHE BOOL "" FORCE)
    set(WITH_SSL OFF CACHE BOOL "" FORCE)
    set(WITH_UI OFF CACHE BOOL "" FORCE)
    set(WITH_UV OFF CACHE BOOL "" FORCE)
    set(WITH_VIDEO OFF CACHE BOOL "" FORCE)
    set(WITH_HEAPS OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
    set(DOWNLOAD_DEPENDENCIES OFF CACHE BOOL "" FORCE)

    add_subdirectory("${HASHLINK_DIR}" hashlink-build EXCLUDE_FROM_ALL)

    if(BUILD_SHARED_LIBS)
        set(HDLL_SUFFIX .hdll)
    else()
        set(HDLL_SUFFIX .hdll${CMAKE_STATIC_LIBRARY_SUFFIX})
    endif()

    function(set_as_hdll target)
        set_target_properties(${target}.hdll
            PROPERTIES
            PREFIX ""
            OUTPUT_NAME ${target}
            SUFFIX ${HDLL_SUFFIX}
        )
        if(NOT MSVC AND NOT APPLE)
            target_link_options(${target}.hdll PRIVATE "-Wl,--no-undefined")
        endif()
    endfunction()

    set(HDLL_DESTINATION ${CMAKE_BINARY_DIR}/bin)
else()
    include(FindPkgConfig)
endif()

add_library(postgres.hdll
    postgres.c
)

set_as_hdll(postgres)

set(LIBPQ_TARGET "")

find_package(PostgreSQL QUIET)
if(TARGET PostgreSQL::PostgreSQL)
    set(LIBPQ_TARGET PostgreSQL::PostgreSQL)
endif()

if(NOT LIBPQ_TARGET AND PkgConfig_FOUND)
    pkg_check_modules(LIBPQ IMPORTED_TARGET libpq)
    if(TARGET PkgConfig::LIBPQ)
        set(LIBPQ_TARGET PkgConfig::LIBPQ)
    endif()
endif()

if(LIBPQ_TARGET)
    target_link_libraries(postgres.hdll
        libhl
        ${LIBPQ_TARGET}
    )
else()
    find_path(LIBPQ_INCLUDE_DIR
        NAMES libpq-fe.h
        PATH_SUFFIXES postgresql
    )
    find_library(LIBPQ_LIBRARY
        NAMES pq libpq
    )

    if(NOT LIBPQ_INCLUDE_DIR OR NOT LIBPQ_LIBRARY)
        message(FATAL_ERROR "libpq was not found. Install PostgreSQL development files or set PostgreSQL_ROOT/CMAKE_PREFIX_PATH.")
    endif()

    target_include_directories(postgres.hdll
        PRIVATE
        ${LIBPQ_INCLUDE_DIR}
    )
    target_link_libraries(postgres.hdll
        libhl
        ${LIBPQ_LIBRARY}
    )
endif()

if(WIN32)
    target_link_libraries(postgres.hdll
        ws2_32
        secur32
        crypt32
    )
endif()

install(
    TARGETS
        postgres.hdll
    DESTINATION ${HDLL_DESTINATION}
)

