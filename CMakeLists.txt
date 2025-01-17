cmake_minimum_required(VERSION 3.30)

project(Motion LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets UiTools)
find_package(CGAL REQUIRED)

set(APP_SOURCE_FOLDER "src")
set(APP_INCLUDE_FOLDER "include")
set(APP_FORMS_FOLDER "forms")
set(APP_RESOURCES_FOLDER "resources")
set(CMAKE_AUTOUIC_SEARCH_PATHS ${APP_FORMS_FOLDER})

file(GLOB_RECURSE APP_HEADERS ${APP_INCLUDE_FOLDER}/*.h)
file(GLOB_RECURSE APP_SOURCES ${APP_SOURCE_FOLDER}/*.cpp)
file(GLOB_RECURSE APP_RESOURCES ${APP_RESOURCES_FOLDER}/*.ico ${APP_RESOURCES_FOLDER}/*.qrc)

if(WIN32)
    file(GLOB_RECURSE WIN_APP_RESOURCES ${APP_RESOURCES_FOLDER}/*.rc)
    set(APP_RESOURCES ${APP_RESOURCES} ${WIN_APP_RESOURCES})
endif()

source_group("Resources" FILES ${APP_RESOURCES})

add_executable(Motion ${APP_HEADERS} ${APP_SOURCES} ${APP_RESOURCES})
target_include_directories(Motion PUBLIC ${APP_INCLUDE_FOLDER})
target_link_libraries(Motion PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets CGAL::CGAL)
set_target_properties(Motion PROPERTIES
    AUTOUIC ON
    AUTOMOC ON
    AUTORCC ON
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)