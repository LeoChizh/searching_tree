# Find GTest
find_package(GTest REQUIRED)

# Create test executable
add_executable(CommandsTest 
    test_commands.cpp
)

# Link test executable with GTest and your library
target_link_libraries(CommandsTest 
    GTest::gtest 
    GTest::gtest_main 
    SearchingTreeLib
)

# Add test
add_test(NAME CommandsTest COMMAND CommandsTest)