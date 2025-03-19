ADD_TARGET(
        MODE STATIC
        SOURCE
        "${PROJECT_SOURCE_DIR}/include/_deps/tinyobjloader"
        INC
        "${PROJECT_SOURCE_DIR}/include"
        INC_PRIVATE
        "${PROJECT_SOURCE_DIR}/include/_deps/tinyobjloader"
        #  DEFINE
        #    TINYOBJLOADER_USE_DOUBLE
)