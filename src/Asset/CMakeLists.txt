DOWNLOAD_ZIP_PRO(
        https://cdn.jsdelivr.net/gh/shimakaze09/MyData@main/MyGE/myshader/MyShaderCompiler_deps_31032025.zip
        MyShaderCompiler_deps_31032025.zip
        ${CMAKE_CURRENT_SOURCE_DIR}/MyShaderCompiler/_deps
        SHA256 1cdc80c4d2fbdcb249f1c30d198782ce9d14e93792775a90441262a09e837a3c
)

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