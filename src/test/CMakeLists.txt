find_package(GTest)

add_executable(
    test_typedef
        test_typedef.cpp
)

target_include_directories(
    test_typedef
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)


target_link_libraries(
    test_typedef
        PRIVATE
            GTest::gtest_main
)


add_test(
    NAME test_typedef
    COMMAND $<TARGET_FILE:test_typedef>
)
