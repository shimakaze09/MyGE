ADD_TARGET(
        MODE STATIC
        SOURCE
        "${PROJECT_SOURCE_DIR}/include/MyGE/Render/DX12"
        INC
        "${PROJECT_SOURCE_DIR}/include"
        LIB
        My::MyDX12_core
        My::MyGE_Core
)