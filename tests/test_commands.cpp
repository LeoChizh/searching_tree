#include <gtest/gtest.h>
#include "Commands.hpp"
#include <climits>
#include <algorithm>
#include <sstream>

// Tests for PUBLIC interface only - black box testing
class CommandsTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}

    Commands cmd;
};

// Basic functionality tests
TEST_F(CommandsTest, DefaultConstruction) {
    SUCCEED();
}

TEST_F(CommandsTest, AddCommandNoValues) {
    cmd.add_command(TreeCommand::add);
    cmd.add_command(TreeCommand::find_min);
    cmd.add_command(TreeCommand::number_smaller);
    SUCCEED();
}

TEST_F(CommandsTest, AddCommandSingleValue) {
    cmd.add_command(TreeCommand::add, {42});
    cmd.add_command(TreeCommand::find_min, {100});
    cmd.add_command(TreeCommand::number_smaller, {7});
    SUCCEED();
}

TEST_F(CommandsTest, AddCommandMultipleValues) {
    cmd.add_command(TreeCommand::add, {10, 20});
    cmd.add_command(TreeCommand::number_smaller, {1, 2, 3, 4, 5});
    SUCCEED();
}

TEST_F(CommandsTest, AddMixedCommands) {
    cmd.add_command(TreeCommand::add);
    cmd.add_command(TreeCommand::find_min, {5});
    cmd.add_command(TreeCommand::number_smaller, {1, 2, 3});
    cmd.add_command(TreeCommand::add, {100, 200});
    SUCCEED();
}

// Output tests
TEST_F(CommandsTest, PrintEmptyCommands) {
    testing::internal::CaptureStdout();
    cmd.print_commands();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.empty());
}

TEST_F(CommandsTest, PrintCommandsOutput) {
    cmd.add_command(TreeCommand::add, {10, 20});
    cmd.add_command(TreeCommand::find_min, {5});
    cmd.add_command(TreeCommand::number_smaller);
    cmd.add_command(TreeCommand::number_smaller, {1, 2, 3});
    
    testing::internal::CaptureStdout();
    cmd.print_commands();
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_TRUE(output.find("add") != std::string::npos);
    EXPECT_TRUE(output.find("find_min") != std::string::npos);
    EXPECT_TRUE(output.find("number_smaller") != std::string::npos);
    EXPECT_TRUE(output.find("10") != std::string::npos);
    EXPECT_TRUE(output.find("20") != std::string::npos);
    EXPECT_TRUE(output.find("5") != std::string::npos);
}

TEST_F(CommandsTest, CommandOrderPreserved) {
    cmd.add_command(TreeCommand::add, {1});
    cmd.add_command(TreeCommand::find_min, {2});
    cmd.add_command(TreeCommand::number_smaller, {3});
    cmd.add_command(TreeCommand::add, {4});
    
    testing::internal::CaptureStdout();
    cmd.print_commands();
    std::string output = testing::internal::GetCapturedStdout();
    
    size_t pos_add1 = output.find("add [1]");
    size_t pos_find_min = output.find("find_min [2]");
    size_t pos_number_smaller = output.find("number_smaller [3]");
    size_t pos_add2 = output.find("add [4]");
    
    EXPECT_TRUE(pos_add1 < pos_find_min);
    EXPECT_TRUE(pos_find_min < pos_number_smaller);
    EXPECT_TRUE(pos_number_smaller < pos_add2);
}

// Edge cases
TEST_F(CommandsTest, EdgeCaseValues) {
    cmd.add_command(TreeCommand::add, {0});
    cmd.add_command(TreeCommand::find_min, {-1});
    cmd.add_command(TreeCommand::number_smaller, {INT_MAX});
    cmd.add_command(TreeCommand::add, {INT_MIN});
    
    testing::internal::CaptureStdout();
    cmd.print_commands();
    std::string output = testing::internal::GetCapturedStdout();
    SUCCEED();
}

// NEW: Test direct command access
TEST_F(CommandsTest, GetCommandsAccess) {
    cmd.add_command(TreeCommand::add, {42});
    cmd.add_command(TreeCommand::find_min);
    
    const auto& commands = cmd.get_commands();
    EXPECT_EQ(commands.size(), 2);
    EXPECT_EQ(commands[0].cmd, TreeCommand::add);
    EXPECT_EQ(commands[0].values[0], 42);
    EXPECT_EQ(commands[1].cmd, TreeCommand::find_min);
    EXPECT_TRUE(commands[1].values.empty());
}

// Parser tests through public interface
TEST_F(CommandsTest, ParserValidFormattedInput) {
    std::istringstream input("k 4 k 3 k 2 m 1 n 1");
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    cmd.parse_from_stdin();
    std::cin.rdbuf(old_cin_buf);
    
    EXPECT_EQ(cmd.size(), 5);
    const auto& commands = cmd.get_commands();
    EXPECT_EQ(commands[0].cmd, TreeCommand::add);
    EXPECT_EQ(commands[0].values[0], 4);
}

// REMOVED: ParserAttachedNumbers test (k4, m1, n1 are now invalid)

TEST_F(CommandsTest, ParserCommandsWithoutValues) {
    std::istringstream input("k m n k 5");
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    cmd.parse_from_stdin();
    std::cin.rdbuf(old_cin_buf);
    
    EXPECT_EQ(cmd.size(), 4);
    const auto& commands = cmd.get_commands();
    EXPECT_TRUE(commands[0].values.empty());
    EXPECT_EQ(commands[3].values[0], 5);
}

TEST_F(CommandsTest, ParserInvalidInput) {
    std::istringstream input("x 1 y 2 z 3 k 4");
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    
    testing::internal::CaptureStderr();
    cmd.parse_from_stdin();
    std::string error_output = testing::internal::GetCapturedStderr();
    std::cin.rdbuf(old_cin_buf);
    
    EXPECT_EQ(cmd.size(), 1);
    EXPECT_TRUE(error_output.find("Unknown token") != std::string::npos);
}

TEST_F(CommandsTest, ParserHandlesMultipleLines) {
    std::istringstream input("k 1 k 2\nm 3\nn 4 k 5");
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    cmd.parse_from_stdin();
    std::cin.rdbuf(old_cin_buf);
    
    EXPECT_EQ(cmd.size(), 5);
}

TEST_F(CommandsTest, ParserHandlesNegativeNumbers) {
    std::istringstream input("k -1 m -42 n -100");
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    cmd.parse_from_stdin();
    std::cin.rdbuf(old_cin_buf);
    
    EXPECT_EQ(cmd.size(), 3);
    const auto& commands = cmd.get_commands();
    EXPECT_EQ(commands[0].values[0], -1);
    EXPECT_EQ(commands[1].values[0], -42);
    EXPECT_EQ(commands[2].values[0], -100);
}

TEST_F(CommandsTest, ParserHandlesZero) {
    std::istringstream input("k 0 m 0 n 0");
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    cmd.parse_from_stdin();
    std::cin.rdbuf(old_cin_buf);
    
    EXPECT_EQ(cmd.size(), 3);
    const auto& commands = cmd.get_commands();
    EXPECT_EQ(commands[0].values[0], 0);
    EXPECT_EQ(commands[1].values[0], 0);
    EXPECT_EQ(commands[2].values[0], 0);
}

TEST_F(CommandsTest, ParserHandlesLargeNumbers) {
    std::istringstream input("k 999999 m 123456 n 789012");
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    cmd.parse_from_stdin();
    std::cin.rdbuf(old_cin_buf);
    
    EXPECT_EQ(cmd.size(), 3);
    const auto& commands = cmd.get_commands();
    EXPECT_EQ(commands[0].values[0], 999999);
    EXPECT_EQ(commands[1].values[0], 123456);
    EXPECT_EQ(commands[2].values[0], 789012);
}

// UPDATED: Remove mixed format test that included attached commands
TEST_F(CommandsTest, ParserOutputFormatConsistency) {
    std::istringstream input("k 4 k 3 k 2 m 1 n 1");
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    cmd.parse_from_stdin();
    std::cin.rdbuf(old_cin_buf);
    
    testing::internal::CaptureStdout();
    cmd.print_commands();
    std::string output = testing::internal::GetCapturedStdout();
    
    // Check consistent output format
    EXPECT_TRUE(output.find("add [4]") != std::string::npos);
    EXPECT_TRUE(output.find("add [3]") != std::string::npos);
    EXPECT_TRUE(output.find("add [2]") != std::string::npos);
    EXPECT_TRUE(output.find("find_min [1]") != std::string::npos);
    EXPECT_TRUE(output.find("number_smaller [1]") != std::string::npos);
}

// Test clear functionality
TEST_F(CommandsTest, ClearCommands) {
    cmd.add_command(TreeCommand::add, {1});
    cmd.add_command(TreeCommand::find_min, {2});
    EXPECT_EQ(cmd.size(), 2);
    
    cmd.clear();
    EXPECT_EQ(cmd.size(), 0);
    
    const auto& commands = cmd.get_commands();
    EXPECT_TRUE(commands.empty());
}

// Tests for multiple values functionality
TEST_F(CommandsTest, ParserHandlesMultipleValues) {
    std::istringstream input("k 4 3 2 m 1 n 5 6");
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    cmd.parse_from_stdin();
    std::cin.rdbuf(old_cin_buf);
    
    EXPECT_EQ(cmd.size(), 3);
    const auto& commands = cmd.get_commands();
    
    // First command: k 4 3 2 → add with values [4, 3, 2]
    EXPECT_EQ(commands[0].cmd, TreeCommand::add);
    EXPECT_EQ(commands[0].values.size(), 3);
    EXPECT_EQ(commands[0].values[0], 4);
    EXPECT_EQ(commands[0].values[1], 3);
    EXPECT_EQ(commands[0].values[2], 2);
    
    // Second command: m 1 → find_min with value [1]
    EXPECT_EQ(commands[1].cmd, TreeCommand::find_min);
    EXPECT_EQ(commands[1].values.size(), 1);
    EXPECT_EQ(commands[1].values[0], 1);
    
    // Third command: n 5 6 → number_smaller with values [5, 6]
    EXPECT_EQ(commands[2].cmd, TreeCommand::number_smaller);
    EXPECT_EQ(commands[2].values.size(), 2);
    EXPECT_EQ(commands[2].values[0], 5);
    EXPECT_EQ(commands[2].values[1], 6);
}

TEST_F(CommandsTest, ParserHandlesMixedSingleAndMultipleValues) {
    std::istringstream input("k 1 k 2 3 m n 4 5 6");
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    cmd.parse_from_stdin();
    std::cin.rdbuf(old_cin_buf);
    
    EXPECT_EQ(cmd.size(), 4);
    const auto& commands = cmd.get_commands();
    
    EXPECT_EQ(commands[0].cmd, TreeCommand::add);
    EXPECT_EQ(commands[0].values.size(), 1);
    EXPECT_EQ(commands[0].values[0], 1);
    
    EXPECT_EQ(commands[1].cmd, TreeCommand::add);
    EXPECT_EQ(commands[1].values.size(), 2);
    EXPECT_EQ(commands[1].values[0], 2);
    EXPECT_EQ(commands[1].values[1], 3);
    
    EXPECT_EQ(commands[2].cmd, TreeCommand::find_min);
    EXPECT_EQ(commands[2].values.size(), 0);
    
    EXPECT_EQ(commands[3].cmd, TreeCommand::number_smaller);
    EXPECT_EQ(commands[3].values.size(), 3);
    EXPECT_EQ(commands[3].values[0], 4);
    EXPECT_EQ(commands[3].values[1], 5);
    EXPECT_EQ(commands[3].values[2], 6);
}

TEST_F(CommandsTest, ParserHandlesMultipleValuesWithNegativeNumbers) {
    std::istringstream input("k 1 -2 3 m -4 -5");
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    cmd.parse_from_stdin();
    std::cin.rdbuf(old_cin_buf);
    
    const auto& commands = cmd.get_commands();
    
    EXPECT_EQ(commands[0].cmd, TreeCommand::add);
    EXPECT_EQ(commands[0].values.size(), 3);
    EXPECT_EQ(commands[0].values[0], 1);
    EXPECT_EQ(commands[0].values[1], -2);
    EXPECT_EQ(commands[0].values[2], 3);
    
    EXPECT_EQ(commands[1].cmd, TreeCommand::find_min);
    EXPECT_EQ(commands[1].values.size(), 2);
    EXPECT_EQ(commands[1].values[0], -4);
    EXPECT_EQ(commands[1].values[1], -5);
}

// NEW: Test that attached formats are rejected
TEST_F(CommandsTest, ParserRejectsAttachedFormats) {
    std::istringstream input("k4 m1 n5");
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    
    testing::internal::CaptureStderr();
    cmd.parse_from_stdin();
    std::string error_output = testing::internal::GetCapturedStderr();
    std::cin.rdbuf(old_cin_buf);
    
    // Should have errors for all attached formats
    EXPECT_TRUE(error_output.find("Unknown token: k4") != std::string::npos);
    EXPECT_TRUE(error_output.find("Unknown token: m1") != std::string::npos);
    EXPECT_TRUE(error_output.find("Unknown token: n5") != std::string::npos);
    
    // No commands should be parsed
    EXPECT_EQ(cmd.size(), 0);
}

TEST_F(CommandsTest, ParserHandlesManyValues) {
    std::istringstream input("k 1 2 3 4 5 6 7 8 9 10");
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    cmd.parse_from_stdin();
    std::cin.rdbuf(old_cin_buf);
    
    const auto& commands = cmd.get_commands();
    
    EXPECT_EQ(commands[0].cmd, TreeCommand::add);
    EXPECT_EQ(commands[0].values.size(), 10);
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(commands[0].values[i], i + 1);
    }
}

TEST_F(CommandsTest, PrintCommandsWithMultipleValues) {
    cmd.add_command(TreeCommand::add, {1, 2, 3});
    cmd.add_command(TreeCommand::find_min, {4, 5});
    cmd.add_command(TreeCommand::number_smaller, {6, 7, 8, 9});
    
    testing::internal::CaptureStdout();
    cmd.print_commands();
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_TRUE(output.find("add [1, 2, 3]") != std::string::npos);
    EXPECT_TRUE(output.find("find_min [4, 5]") != std::string::npos);
    EXPECT_TRUE(output.find("number_smaller [6, 7, 8, 9]") != std::string::npos);
}

TEST_F(CommandsTest, PrintCommandsWithEmptyValues) {
    cmd.add_command(TreeCommand::add, {});
    cmd.add_command(TreeCommand::find_min, {});
    cmd.add_command(TreeCommand::number_smaller, {});
    
    testing::internal::CaptureStdout();
    cmd.print_commands();
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_TRUE(output.find("add []") != std::string::npos);
    EXPECT_TRUE(output.find("find_min []") != std::string::npos);
    EXPECT_TRUE(output.find("number_smaller []") != std::string::npos);
}

TEST_F(CommandsTest, ValuesAreImmutableAfterParsing) {
    std::istringstream input("k 1 2 3");
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    cmd.parse_from_stdin();
    std::cin.rdbuf(old_cin_buf);
    
    const auto& commands = cmd.get_commands();
    
    // Verify values are stored correctly and cannot be modified
    EXPECT_EQ(commands[0].values.size(), 3);
    EXPECT_EQ(commands[0].values[0], 1);
    EXPECT_EQ(commands[0].values[1], 2);
    EXPECT_EQ(commands[0].values[2], 3);
    
    // The const qualifier ensures immutability at compile time
    SUCCEED();
}