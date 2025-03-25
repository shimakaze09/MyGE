SET(components
        Camera
        MeshFilter
        MeshRenderer
        Skybox
        Light
)

SET(refls "")

FOREACH (cmpt ${components})
    SET(dst "${PROJECT_SOURCE_DIR}/include/MyGE/Render/Components/details/${cmpt}_AutoRefl.inl")
    MYSREFL_AUTO_REFL(
            "${PROJECT_SOURCE_DIR}/include/MyGE/Render/Components/${cmpt}.h"
            ${dst}
    )
    LIST(APPEND refls ${dst})
ENDFOREACH ()

DOWNLOAD_ZIP_PRO(
        https://cdn.jsdelivr.net/gh/shimakaze09/MyData@main/MyGE/LTCTex.zip
        LTCTex.zip
        "${CMAKE_CURRENT_SOURCE_DIR}/_deps"
        SHA256 e03bda5817c6258a8c31a84d5b30385c8fb97dac827b0b708d908994ee419b4b
)

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
)