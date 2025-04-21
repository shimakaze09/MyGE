MACRO(INIT_MYCMAKE)
    CMAKE_PARSE_ARGUMENTS(
            "ARG" # prefix
            "" #<options> # TRUE / FALSE
            "VERSION" # <one_value_keywords>
            "" #<multi_value_keywords> # list
            ${ARGN}
    )
    # The results are in ARG_*
    # - ARG_<option>
    # - ARG_<one_value_keyword>
    # - ARG_<multi_value_keyword>

    IF ("${ARG_VERSION}" STREQUAL "")
        MESSAGE(FATAL_ERROR "Not provide argument <VERSION>")
    ENDIF ()

    INCLUDE(FETCHCONTENT)
    SET(MyCMake_VERSION ${ARG_VERSION})
    MESSAGE(STATUS "Looking for: MyCMake ${MyCMake_VERSION}")
    FIND_PACKAGE(MyCMake ${MyCMake_VERSION} EXACT QUIET)
    IF (NOT MyCMake_FOUND)
        SET(ADDRESS "https://github.com/shimakaze09/MyCMake.git")
        MESSAGE(STATUS "MyCMake ${MyCMake_VERSION} not found\n")
        MESSAGE(STATUS "Fetching from ${ADDRESS} with tag ${MyCMake_VERSION}")
        FETCHCONTENT_DECLARE(
                MyCMake
                GIT_REPOSITORY ${ADDRESS}
                GIT_TAG ${MyCMake_VERSION}
        )
        FETCHCONTENT_MAKEAVAILABLE(MyCMake)
        MESSAGE(STATUS "MyCMake ${MyCMake_VERSION} built")
    ENDIF ()
ENDMACRO()