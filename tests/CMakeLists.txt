# Find GTest
find_package(GTest REQUIRED)

# Test executables
add_executable(CommandsTest test_commands.cpp)
add_executable(NodeTest test_node.cpp)

# Link with GTest and your library
target_link_libraries(CommandsTest GTest::gtest GTest::gtest_main SearchingTreeLib)
target_link_libraries(NodeTest GTest::gtest GTest::gtest_main SearchingTreeLib)

# Add tests
add_test(NAME CommandsTest COMMAND CommandsTest)
add_test(NAME NodeTest COMMAND NodeTest)