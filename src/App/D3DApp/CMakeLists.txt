ADD_TARGET(
        TEST
        MODE STATIC
        SOURCE
        "${PROJECT_SOURCE_DIR}/include/MyGE/App/D3DApp"
        LIB
        My::MyGE_Render_DX12
        My::MyGE_Asset
        L_OPTION_INTERFACE
        /SUBSYSTEM:WINDOWS
)