cmake_minimum_required(VERSION 3.16)

project(myConvolution VERSION 0.1 LANGUAGES CXX)

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
)

set(PROJECT_SOURCES
    main.cpp
    qml.qrc

    backend.h
    backend.cpp

    databasemanager.h
    databasemanager.cpp

    databaseworker.h
    databaseworker.cpp

    peak.h peak.cpp
    noise.h noise.cpp
)

add_executable(myConvolution
    ${PROJECT_SOURCES}
)

target_link_libraries(myConvolution
    PRIVATE
        Qt5::Core
        Qt5::Gui
        Qt5::Quick
        Qt5::Qml
        Qt5::Sql
)

set_target_properties(myConvolution PROPERTIES
    MACOSX_BUNDLE TRUE
)
