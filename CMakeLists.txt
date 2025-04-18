CMAKE_MINIMUM_REQUIRED(VERSION 3.20 FATAL_ERROR)

PROJECT(MyGE VERSION 0.0.5)
MESSAGE(STATUS "Project: ${PROJECT_NAME} ${PROJECT_VERSION}")

INCLUDE(cmake/InitMyCMake.cmake)
INIT_MYCMAKE()

INIT_PROJECT()

SET(MySRefl_Build_AutoRefl TRUE CACHE BOOL "use auto refl" FORCE)

ADD_DEP(MyRapidJSON 0.0.3)
ADD_DEP(MyDP 0.7.4)
ADD_DEP(MyGM 0.6.13)
ADD_DEP(MyLuaPP 0.2.0)
ADD_DEP(MyECS 0.14.4)
ADD_DEP(MyDX12 0.0.12)

DOWNLOAD_ZIP(
        https://cdn.jsdelivr.net/gh/shimakaze09/MyData@main/MyGE/crossguid/crossguid_19032025.zip
        crossguid_19032025.zip
        SHA256 1BEE5F2424612E3EF3684D139D6038EF2420EAC7687BF210057E752EFE2A05D5
)

DOWNLOAD_ZIP(
        https://cdn.jsdelivr.net/gh/shimakaze09/MyData@main/MyGE/imgui/imgui_dock_1_78_0.zip
        imgui_dock_1_78_0.zip
        SHA256 E57BA9CD40CA2113BEF0BC1864C3B8BC0B9A97C39F50DA02AAF251CE0A168F0C
)

DOWNLOAD_ZIP(
        https://cdn.jsdelivr.net/gh/shimakaze09/MyData@main/MyGE/spdlog/spdlog_1_8_1.zip
        spdlog_1_8_1.zip
        SHA256 2898A66A808A92C546BC5C173423BB5EA84BA0278A897D6F03DCFC1A6F98D866
)

ADD_SUB_DIRS_REC(include)
ADD_SUB_DIRS_REC(src)

EXPORT_TARGETS(
        TARGET
        DIRECTORIES
        "include"
)