add_executable(generator_tests generator_test.cpp pole_peak_test.cpp sphere_grid_test.cpp)
target_link_libraries(generator_tests PRIVATE generator GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(generator_tests)
