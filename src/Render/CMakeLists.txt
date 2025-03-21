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