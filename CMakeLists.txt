CMAKE_MINIMUM_REQUIRED(VERSION 3.20 FATAL_ERROR)

PROJECT(MyGE VERSION 0.0.1)
MESSAGE(STATUS "Project: ${PROJECT_NAME} ${PROJECT_VERSION}")

INCLUDE(cmake/InitMyCMake.cmake)
INIT_MYCMAKE()

INIT_PROJECT()

ADD_DEP(MyRapidJSON 0.0.1)
ADD_DEP(MySTL 0.1.1)
ADD_DEP(MyDP 0.7.1)
ADD_DEP(MyGM 0.6.9)
ADD_DEP(MyLuaPP 0.1.4)
ADD_DEP(MyECS 0.12.0)
ADD_DEP(MyDX12 0.0.8)

DOWNLOAD_ZIP(
        https://cdn.jsdelivr.net/gh/shimakaze09/MyData@main/MyGE/crossguid/crossguid_15032025.zip
        crossguid.zip
        SHA256 1d9584217221dcad377df79eae2fe9fb1c3b6fc762499401b819a625e5b81bbe
)

DOWNLOAD_ZIP(
        https://cdn.jsdelivr.net/gh/shimakaze09/MyData@1e0df35fcca08cf523f8213d167424e56bf648ed/MyGE/imgui/imgui_1_78.zip
        imgui_1_78.zip
        SHA256 84c97aa5837717bc18cc106257f284694adce0b84c8dfbf327294bd3c1c35e31
)

ADD_SUB_DIRS_REC(include)
ADD_SUB_DIRS_REC(src)

EXPORT_TARGETS(DIRECTORIES "include")