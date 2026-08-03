cmake_minimum_required(VERSION 4.2)
project(testField)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 5.15 REQUIRED COMPONENTS
        Core
        Gui
        Quick
        Qml
        Sql
        Positioning
)


add_executable(testField main.cpp)

target_link_libraries(testField
        PRIVATE
        Qt5::Core
        Qt5::Gui
        Qt5::Quick
        Qt5::Qml
        Qt5::Sql
        Qt5::Positioning
)

set_target_properties(testField PROPERTIES
        MACOSX_BUNDLE TRUE
)

target_compile_options(testField PRIVATE
        -Wall
        -Wextra
        -Wimplicit-fallthrough
)
