DOWNLOAD_FILE(
        https://cdn.jsdelivr.net/gh/shimakaze09/MyData@main/common/include/stb_image_write.h
        ${CMAKE_CURRENT_SOURCE_DIR}/_deps/stb_image_write.h
        SHA256 cbd5f0ad7a9cf4468affb36354a1d2338034f2c12473cf1a8e32053cb6914a05
)

DOWNLOAD_FILE(
        https://cdn.jsdelivr.net/gh/shimakaze09/MyData@main/common/include/stb_image.h
        ${CMAKE_CURRENT_SOURCE_DIR}/_deps/stb_image.h
        SHA256 594c2fe35d49488b4382dbfaec8f98366defca819d916ac95becf3e75f4200b3
)

SET(components
        Camera
        MeshFilter
        MeshRenderer
        WorldTime
        Skybox
        Light
        Input
        Roamer
)

SET(refls "")

FOREACH (cmpt ${components})
    SET(dst "${PROJECT_SOURCE_DIR}/include/MyGE/Core/Components/details/${cmpt}_AutoRefl.inl")
    MYSREFL_AUTO_REFL(
            "${PROJECT_SOURCE_DIR}/include/MyGE/Core/Components/${cmpt}.h"
            ${dst}
    )
    LIST(APPEND refls ${dst})
ENDFOREACH ()

ADD_TARGET(
        MODE STATIC
        SOURCE
        "${PROJECT_SOURCE_DIR}/include/MyGE/Core"
        ${refls}
        INC
        "${PROJECT_SOURCE_DIR}/include"
        LIB
        My::MyGM_core
        My::MyDP_core
        My::MyGE_Transform
        My::MyGE__deps_imgui
        DEFINE
        NOMINMAX
)