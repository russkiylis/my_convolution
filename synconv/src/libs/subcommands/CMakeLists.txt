add_library(subcommands STATIC
        src/subcommand.h src/subcommand.cpp
        src/createconfigsubcommand.h src/createconfigsubcommand.cpp
        src/generatesubcommand.h src/generatesubcommand.cpp
    )

target_include_directories(subcommands PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(subcommands PUBLIC
    CLI11
    generator
    global
)

#if(ENABLE_GTEST)
#    add_subdirectory(tests)
#endif()