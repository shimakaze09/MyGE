ADD_TARGET(
        MODE STATIC
        SOURCE
        "${PROJECT_SOURCE_DIR}/include/_deps/imgui"
        INC
        "${PROJECT_SOURCE_DIR}/include"
        INC_PRIVATE
        "${PROJECT_SOURCE_DIR}/include/_deps/imgui"
        "${PROJECT_SOURCE_DIR}/include/_deps/imgui/misc/cpp"
        LIB
        My::MyGM_core
)