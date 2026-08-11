cmake_minimum_required(VERSION 3.16)

project(myConvolution VERSION 0.1 LANGUAGES C CXX)

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

set(PROJECT_SOURCES
    main.cpp
    qml.qrc

    backend.h backend.cpp
    databasemanager.h databasemanager.cpp
    databaseworker.h databaseworker.cpp

    peak.h peak.cpp
    noise.h noise.cpp
    loadgenerator.h loadgenerator.cpp
    generatorbackend.h generatorbackend.cpp
    abstractnoisebackend.h abstractnoisebackend.cpp
    normalnoisebackend.h normalnoisebackend.cpp
    uniformnoisebackend.h uniformnoisebackend.cpp
    peaklistmodel.h peaklistmodel.cpp
    postlistmodel.h postlistmodel.cpp
    utils.h utils.cpp
    savebackend.h savebackend.cpp
    visualizebackend.h visualizebackend.cpp
    visualizationtablemodel.h visualizationtablemodel.cpp
    dataforvisualization.h
    bytearraycoder.h bytearraycoder.cpp
    geometryutils.cpp geometryutils.h
)

set(PACKEDARRAY_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/PackedArray
)

set(PACKEDARRAY_SOURCES
        ${PACKEDARRAY_DIR}/packedarray.h ${PACKEDARRAY_DIR}/packedarray.c
)

add_executable(myConvolution
    ${PROJECT_SOURCES}
    ${PACKEDARRAY_SOURCES}
)

target_link_libraries(myConvolution
    PRIVATE
        Qt5::Core
        Qt5::Gui
        Qt5::Quick
        Qt5::Qml
        Qt5::Sql
        Qt5::Positioning
)

set_target_properties(myConvolution PROPERTIES
    MACOSX_BUNDLE TRUE
)

target_compile_options(myConvolution PRIVATE
        -Wall
        -Wextra
        -Wimplicit-fallthrough
        -Wshadow
        -Wformat=2
        -Wundef
        -Wold-style-cast
)

target_compile_options(myConvolution  PRIVATE
    "$<$<CONFIG:Release>:-O3>"
    "$<$<CONFIG:Release>:-ffast-math>"
)


include(CheckIPOSupported)
check_ipo_supported(
        RESULT ipo_supported
        OUTPUT ipo_error
        LANGUAGES CXX
)

if(ipo_supported)
    set_property(
            TARGET myConvolution
            PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE
    )
endif()


if(MSVC)
    target_compile_options(myConvolution PRIVATE
            "$<$<CONFIG:Release>:/arch:AVX2>"
    )
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
    target_compile_options(myConvolution PRIVATE
            "$<$<CONFIG:Release>:-mcpu=native>"
    )
else()
    target_compile_options(myConvolution PRIVATE
            "$<$<CONFIG:Release>:-march=native>"
    )
endif()