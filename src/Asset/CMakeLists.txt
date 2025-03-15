ADD_TARGET(
        MODE STATIC
        SOURCE
        "${PROJECT_SOURCE_DIR}/include/MyGE/Asset"
        INC
        "${PROJECT_SOURCE_DIR}/include"
        LIB
        My::MyRapidJSON_core
        My::MyGE__deps_crossguid
)