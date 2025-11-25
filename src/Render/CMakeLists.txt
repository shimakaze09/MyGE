DOWNLOAD_ZIP_PRO(
        https://cdn.jsdelivr.net/gh/shimakaze09/MyData@main/MyGE/LTCTex.zip
        LTCTex.zip
        "${CMAKE_CURRENT_SOURCE_DIR}/_deps"
        SHA256 e03bda5817c6258a8c31a84d5b30385c8fb97dac827b0b708d908994ee419b4b
)

DOWNLOAD_ZIP_PRO(
        https://cdn.jsdelivr.net/gh/shimakaze09/MyData@main/MyGE/myshader/MyShaderCompiler_deps_31032025.zip
        MyShaderCompiler_deps_31032025.zip
        ${CMAKE_CURRENT_SOURCE_DIR}/MyShaderCompiler/_deps
        SHA256 1cdc80c4d2fbdcb249f1c30d198782ce9d14e93792775a90441262a09e837a3c
)

SET(libs_private "")
SET(defines_private "")

FIND_PACKAGE(assimp QUIET)
IF (TARGET assimp::assimp)
  LIST(APPEND libs_private assimp::assimp)
  LIST(APPEND defines_private My_MYGE_USE_ASSIMP)
ELSE ()
  MESSAGE(STATUS "[My/Asset] not found assimp")
ENDIF ()

SET(c_options_private "")
IF (MSVC)
  LIST(APPEND c_options_private "/MP;/wd4244;/wd4804")
ELSEIF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  #
ELSEIF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  #
ENDIF ()

ADD_TARGET(
        MODE STATIC
        SOURCE
        "${PROJECT_SOURCE_DIR}/include/MyGE/Render"
        ${refls}
        INC
        "${PROJECT_SOURCE_DIR}/include"
        LIB
        My::MyDX12_core
        My::MyGE_Core
        My::MyANTLR_runtime
        My::MyGE__deps_tinyobjloader
        LIB_PRIVATE
        ${libs_private}
        C_OPTION_PRIVATE
        ${c_options_private}
        DEFINE_PRIVATE
        ${defines_private}
)
