ADD_TARGET(
        MODE STATIC
        SOURCE
        "${PROJECT_SOURCE_DIR}/include/MyGE/LuaECS"
        INC
        "${PROJECT_SOURCE_DIR}/include"
        LIB
        My::MyLuaPP_core
        My::MyECS_core
)