add_executable(subcommands_tests
)

target_link_libraries(subcommands_tests PRIVATE
        subcommands
        GTest::gtest_main
)


include(GoogleTest)
gtest_discover_tests(subcommands_tests)