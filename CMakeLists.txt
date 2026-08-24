cmake_minimum_required(VERSION 3.16)
project(StarModManager VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2 QuickDialogs2)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LibArchive REQUIRED IMPORTED_TARGET libarchive)

qt_standard_project_setup(REQUIRES 6.5)
qt_policy(SET QTP0004 NEW)

qt_add_executable(StarModManager
    src/main.cpp
    src/core/config/configmanager.cpp
    src/core/config/configmanager.h
    src/core/config/gamefinder.cpp
    src/core/config/gamefinder.h
    src/core/mods/archiveextractor.cpp
    src/core/mods/archiveextractor.h
    src/core/mods/modinstaller.cpp
    src/core/mods/modinstaller.h
    src/core/mods/modlistmodel.cpp
    src/core/mods/modlistmodel.h
    src/core/mods/modlist.cpp
    src/core/mods/modlist.h
    src/core/mods/modlistmanager.cpp
    src/core/mods/modlistmanager.h
    src/core/game/gamelauncher.cpp
    src/core/game/gamelauncher.h
    src/core/game/savemanager.cpp
    src/core/game/savemanager.h
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
        qml/pages/ModsPage.qml
        qml/pages/ModListsPage.qml
)

target_link_libraries(StarModManager PRIVATE
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::QuickDialogs2
    PkgConfig::LibArchive
)
