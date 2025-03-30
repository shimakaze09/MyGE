# deps
SET(libs "")
SET(defines "")

FIND_PACKAGE(assimp QUIET)
IF (TARGET assimp::assimp)
    LIST(APPEND libs assimp::assimp)
    LIST(APPEND defines MY_MYGE_USE_ASSIMP)
ELSE ()
    MESSAGE(STATUS "[MyGE/Asset] not found assimp")
ENDIF ()

# target
ADD_TARGET(
        MODE STATIC
        SOURCE
        "${PROJECT_SOURCE_DIR}/include/MyGE/Asset"
        ${myshader_parser_files}
        INC
        "${PROJECT_SOURCE_DIR}/include"
        LIB
        My::MyRapidJSON_core
        My::MyGE__deps_crossguid
        My::MyGE__deps_tinyobjloader
        My::MyGE_ScriptSystem
        ${libs}
        LIB_PRIVATE
        My::MyANTLR_runtime
        DEFINE_PRIVATE
        ${defines}
)