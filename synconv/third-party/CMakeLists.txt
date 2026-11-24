if (ENABLE_GTEST)
    add_subdirectory(
            "googletest"
            EXCLUDE_FROM_ALL
    )

    foreach(_gtest_target IN ITEMS gtest gtest_main gmock gmock_main)
        if(TARGET ${_gtest_target})
            target_compile_options(${_gtest_target} PRIVATE
                    "$<$<CXX_COMPILER_ID:MSVC>:/w>"
                    "$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-w>"
            )
        endif()
    endforeach()
    unset(_gtest_target)

endif()

add_subdirectory(
        "cli11"
)