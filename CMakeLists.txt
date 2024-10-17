cmake_minimum_required(VERSION 3.30)

project(Motion LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets UiTools)
find_package(CGAL REQUIRED)

set(APP_SOURCE_FOLDER "src")
file(GLOB_RECURSE APP_HEADERS ${APP_SOURCE_FOLDER}/*.h)
file(GLOB_RECURSE APP_SOURCES ${APP_SOURCE_FOLDER}/*.cpp)
add_executable(Motion ${APP_HEADERS} ${APP_SOURCES} ${APP_UI_HEADERS})
target_include_directories(Motion PUBLIC ${APP_SOURCE_FOLDER})
target_link_libraries(Motion PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets CGAL::CGAL)
set_target_properties(Motion PROPERTIES
    AUTOUIC ON
    AUTOMOC ON
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)