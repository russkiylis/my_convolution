cmake_minimum_required(VERSION 3.16)

project(synconv
        VERSION 0.1.0
        DESCRIPTION "CLI-реализация генератора синтетических свёрток"
        LANGUAGES CXX
)

# Установка стандарта C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME}
        main.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    subcommands
)

# Добавление флагов компиляции
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # Для Visual Studio
    add_compile_options(/W4)
    add_compile_options(/utf-8)
else()
    add_compile_options(
            -Wall
            -Wextra
            -Wimplicit-fallthrough
            -Wshadow
            -Wformat=2
            -Wundef
            -Wold-style-cast
    )
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
)

# Опция о том, собирать ли тесты
option(ENABLE_GTEST "Включить юнит-тестирование в сборку" OFF)

if(ENABLE_GTEST)
    enable_testing()
endif()

add_subdirectory(libs)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../third-party" "${CMAKE_CURRENT_BINARY_DIR}/third-party")