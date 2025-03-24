FIND_PACKAGE(Threads REQUIRED)

SET(SPDLOG_VERSION 1.8.1)
SET(SPDLOG_VERSION_MAJOR 1)

# use fmt as default
# build static as default

ADD_TARGET(
        MODE STATIC
        RET_TARGET_NAME spdlog
        SOURCE
        "${PROJECT_SOURCE_DIR}/include/_deps/spdlog"
        DEFINE
        SPDLOG_COMPILED_LIB
        INC
        "${PROJECT_SOURCE_DIR}/include/_deps"
        LIB
        Threads::Threads
)

SET_TARGET_PROPERTIES(${spdlog} PROPERTIES VERSION ${SPDLOG_VERSION} SOVERSION ${SPDLOG_VERSION_MAJOR})

IF (COMMAND target_precompile_headers)
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/pch.h.in ${PROJECT_BINARY_DIR}/spdlog_pch.h @ONLY)
    TARGET_PRECOMPILE_HEADERS(${spdlog} PRIVATE ${PROJECT_BINARY_DIR}/spdlog_pch.h)
ENDIF ()