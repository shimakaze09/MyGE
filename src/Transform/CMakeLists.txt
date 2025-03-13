SET(components
        Children
        LocalToParent
        LocalToWorld
        Parent
        Rotation
        RotationEuler
        Scale
        Translation
        WorldToLocal
)

SET(refls "")

FOREACH (cmpt ${components})
    SET(dst "${PROJECT_SOURCE_DIR}/include/MyGE/Transform/Components/details/${cmpt}_AutoRefl.inl")
    MYSREFL_AUTO_REFL(
            "${PROJECT_SOURCE_DIR}/include/MyGE/Transform/Components/${cmpt}.h"
            ${dst}
    )
    LIST(APPEND refls ${dst})
ENDFOREACH ()

ADD_TARGET(
        MODE STATIC
        SOURCE
        "${PROJECT_SOURCE_DIR}/include/MyGE/Transform"
        ${refls}
        INC
        "${PROJECT_SOURCE_DIR}/include"
        LIB
        My::MyGM_core
        My::MyECS_core
        My::MySRefl_core
)