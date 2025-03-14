ADD_TARGET(
        MODE STATIC
        SOURCE
        "${PROJECT_SOURCE_DIR}/include/MyGE/_deps/crossguid"
        INC
        "${PROJECT_SOURCE_DIR}/include"
        INC_PRIVATE
        "${PROJECT_SOURCE_DIR}/include/MyGE/_deps"
        DEFINE_PRIVATE
        GUID_WINDOWS
)