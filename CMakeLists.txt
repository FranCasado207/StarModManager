cmake_minimum_required(VERSION 3.16)
project(StarModManager VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2 QuickDialogs2)

qt_policy(SET QTP0004 NEW)

qt_standard_project_setup(REQUIRES 6.5)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

qt_add_executable(StarModManager
    src/main.cpp
    src/core/config/configmanager.cpp
    src/core/config/configmanager.h
    src/core/config/gamefinder.h
    src/core/config/gamefinder.cpp
)

target_include_directories(StarModManager PRIVATE src)

qt_add_qml_module(StarModManager
    URI com.starmodmanager.app
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/components/Sidebar.qml
        qml/pages/HomePage.qml
        qml/pages/SettingsPage.qml
)

target_link_libraries(StarModManager PRIVATE
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::QuickDialogs2
)
