cmake_minimum_required(VERSION 3.16)
project(Acoustimeter VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Charts
)

qt_standard_project_setup()

set(SOURCES
    src/main.cpp
    src/LoginWindow.cpp
    src/MainWindow.cpp
    src/ConfigWindow.cpp
    src/HydroWidget.cpp
    src/AmplifierWidget.cpp
    src/ToolsWidget.cpp
    src/GraphWidget.cpp
    src/StyleManager.cpp
)

set(HEADERS
    include/LoginWindow.h
    include/MainWindow.h
    include/ConfigWindow.h
    include/HydroWidget.h
    include/AmplifierWidget.h
    include/ToolsWidget.h
    include/GraphWidget.h
    include/StyleManager.h
    include/DataTypes.h
)

set(UI_FILES
    src/LoginWindow.ui
    src/MainWindow.ui
)

qt_add_executable(Acoustimeter
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
)

target_include_directories(Acoustimeter PRIVATE include)

target_link_libraries(Acoustimeter PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Charts
)

set_target_properties(Acoustimeter PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
