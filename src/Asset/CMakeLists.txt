# myshader
SET(target_dir "${PROJECT_SOURCE_DIR}/src/Asset/ShaderCompiler/_deps")
IF (NOT EXISTS ${target_dir})
    ANTLR4_GENERATE(
            GEN_VISITOR
            GEN_LISTENER
            G4 "${PROJECT_SOURCE_DIR}/cmake/Shader.g4"
            NAMESPACE My::MyGE::details
            DIR ${target_dir}
    )
ENDIF ()

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

ADD_TARGET(
        MODE STATIC
        SOURCE
        "${PROJECT_SOURCE_DIR}/include/MyGE/Asset"
        "${PROJECT_SOURCE_DIR}/cmake/Shader.g4"
        ${ushader_parser_files}
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