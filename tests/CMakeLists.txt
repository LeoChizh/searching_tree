# Find GTest
find_package(GTest REQUIRED)

# Test executables
add_executable(CommandsTest_Public test_commands_public.cpp)
add_executable(CommandsTest_Private test_commands_private.cpp)

# Link with GTest and your library
target_link_libraries(CommandsTest_Public GTest::gtest GTest::gtest_main SearchingTreeLib)
target_link_libraries(CommandsTest_Private GTest::gtest GTest::gtest_main SearchingTreeLib)

# Add tests
add_test(NAME CommandsTest_Public COMMAND CommandsTest_Public)
add_test(NAME CommandsTest_Private COMMAND CommandsTest_Private)