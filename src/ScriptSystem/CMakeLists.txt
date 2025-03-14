ADD_TARGET(
        MODE STATIC
        SOURCE
        "${PROJECT_SOURCE_DIR}/include/MyGE/ScriptSystem"
        INC
        "${PROJECT_SOURCE_DIR}/include"
        LIB
        My::MyLuaPP_core
        My::MyECS_core
        DEFINE_PRIVATE
        _CRT_SECURE_NO_WARNINGS
        C_OPTION_PRIVATE
        /bigobj
)