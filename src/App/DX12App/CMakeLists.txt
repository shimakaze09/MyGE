ADD_TARGET(
        MODE STATIC
        SOURCE
        "${PROJECT_SOURCE_DIR}/include/MyGE/App/DX12App"
        LIB
        My::MyGE_Render
        L_OPTION_INTERFACE
        /SUBSYSTEM:WINDOWS
)