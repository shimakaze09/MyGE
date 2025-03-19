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
ADD_DEP(MyLuaPP 0.1.6)
ADD_DEP(MyECS 0.12.2)
ADD_DEP(MyDX12 0.0.8)

DOWNLOAD_ZIP(
        https://cdn.jsdelivr.net/gh/shimakaze09/MyData@main/MyGE/crossguid/crossguid_15032025.zip
        crossguid.zip
        SHA256 1d9584217221dcad377df79eae2fe9fb1c3b6fc762499401b819a625e5b81bbe
)

DOWNLOAD_ZIP(
        https://cdn.jsdelivr.net/gh/shimakaze09/MyData@main/MyGE/imgui/imgui_1_78_2.zip
        imgui_1_78_2.zip
        SHA256 9c6a15ed489baef0427af51f972fa36d33373e229d65d756adcc761b22f6a22a
)

ADD_SUB_DIRS_REC(include)
ADD_SUB_DIRS_REC(src)

EXPORT_TARGETS(DIRECTORIES "include")