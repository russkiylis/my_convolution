add_library(generator STATIC
        src/generator.h
        src/generator.cpp
)

target_include_directories(generator PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(generator PUBLIC
    global
)

target_compile_features(generator PUBLIC cxx_std_20)

if(ENABLE_GTEST)
    add_subdirectory(tests)
endif()

if(ENABLE_GENERATOR_VIEWER)
    add_subdirectory(misc)
endif()
