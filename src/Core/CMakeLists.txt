DOWNLOAD_FILE(
        https://cdn.jsdelivr.net/gh/shimakaze09/MyData@main/common/include/stb_image.h
        ${CMAKE_CURRENT_SOURCE_DIR}/_deps/stb_image.h
        SHA256 594c2fe35d49488b4382dbfaec8f98366defca819d916ac95becf3e75f4200b3
)

DOWNLOAD_FILE(
        https://cdn.jsdelivr.net/gh/shimakaze09/MyData@main/common/include/stb_image_write.h
        ${CMAKE_CURRENT_SOURCE_DIR}/_deps/stb_image_write.h
        SHA256 cbd5f0ad7a9cf4468affb36354a1d2338034f2c12473cf1a8e32053cb6914a05
)

SET(c_options_private "")
IF (MSVC)
  LIST(APPEND c_options_private "/MP;/wd4244")
ELSEIF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  #
ELSEIF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  #
ENDIF ()

SET(c_options "")
IF (MSVC)
  LIST(APPEND c_options "/wd5222")
ELSEIF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  #
ELSEIF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  #
ENDIF ()

ADD_TARGET(
        MODE STATIC
        SOURCE
        "${PROJECT_SOURCE_DIR}/include/MyGE/Core"
        ${refls}
        INC
        "${PROJECT_SOURCE_DIR}/include"
        LIB
        My::MyGM_core
        My::MyVisitor_core
        My::MyDRefl_core
        My::MyECS_core
        My::MyDX12_core
        My::MyRapidJSON_core
        My::MyGE__deps_imgui
        My::MyGE__deps_spdlog
        My::MyGE__deps_crossguid
        C_OPTION
        ${c_options}
        C_OPTION_PRIVATE
        ${c_options_private}
        DEFINE
        NOMINMAX
        MY_MYDREFL_INCLUDE_ALL_STD_NAME
)