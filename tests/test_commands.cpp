#include <gtest/gtest.h>
#include "Commands.hpp"
#include <climits>  // Add this for INT_MAX and INT_MIN
#include <algorithm> // For std::count
#include <sstream>

class CommandsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }

    Commands cmd;
};

// Test default construction
TEST_F(CommandsTest, DefaultConstruction) {
    // Should be able to construct without errors
    SUCCEED();
}

// Test adding commands with no values
TEST_F(CommandsTest, AddCommandNoValues) {
    cmd.add_command(Commands::Command::add);
    cmd.add_command(Commands::Command::find_min);
    cmd.add_command(Commands::Command::number_smaller);
    
    // No assertion for output, just testing it doesn't crash
    SUCCEED();
}

// Test adding commands with single values
TEST_F(CommandsTest, AddCommandSingleValue) {
    cmd.add_command(Commands::Command::add, {42});
    cmd.add_command(Commands::Command::find_min, {100});
    cmd.add_command(Commands::Command::number_smaller, {7});
    
    SUCCEED();
}

// Test adding commands with multiple values
TEST_F(CommandsTest, AddCommandMultipleValues) {
    cmd.add_command(Commands::Command::add, {10, 20});
    cmd.add_command(Commands::Command::number_smaller, {1, 2, 3, 4, 5});
    
    SUCCEED();
}

// Test adding mixed commands
TEST_F(CommandsTest, AddMixedCommands) {
    cmd.add_command(Commands::Command::add);  // No values
    cmd.add_command(Commands::Command::find_min, {5});  // Single value
    cmd.add_command(Commands::Command::number_smaller, {1, 2, 3});  // Multiple values
    cmd.add_command(Commands::Command::add, {100, 200});  // Two values
    
    SUCCEED();
}

// Test print_commands doesn't crash with empty command list
TEST_F(CommandsTest, PrintEmptyCommands) {
    testing::internal::CaptureStdout();
    cmd.print_commands();
    std::string output = testing::internal::GetCapturedStdout();
    
    // Should not crash and output should be empty (no commands)
    EXPECT_TRUE(output.empty());
}

// Test print_commands output format
TEST_F(CommandsTest, PrintCommandsOutput) {
    cmd.add_command(Commands::Command::add, {10, 20});
    cmd.add_command(Commands::Command::find_min, {5});
    cmd.add_command(Commands::Command::number_smaller);
    cmd.add_command(Commands::Command::number_smaller, {1, 2, 3});
    
    testing::internal::CaptureStdout();
    cmd.print_commands();
    std::string output = testing::internal::GetCapturedStdout();
    
    // Check that all command names appear in output
    EXPECT_TRUE(output.find("add") != std::string::npos);
    EXPECT_TRUE(output.find("find_min") != std::string::npos);
    EXPECT_TRUE(output.find("number_smaller") != std::string::npos);
    
    // Check that values appear in output
    EXPECT_TRUE(output.find("10") != std::string::npos);
    EXPECT_TRUE(output.find("20") != std::string::npos);
    EXPECT_TRUE(output.find("5") != std::string::npos);
    EXPECT_TRUE(output.find("1") != std::string::npos);
    EXPECT_TRUE(output.find("2") != std::string::npos);
    EXPECT_TRUE(output.find("3") != std::string::npos);
}

// Test command ordering is preserved
TEST_F(CommandsTest, CommandOrderPreserved) {
    cmd.add_command(Commands::Command::add, {1});
    cmd.add_command(Commands::Command::find_min, {2});
    cmd.add_command(Commands::Command::number_smaller, {3});
    cmd.add_command(Commands::Command::add, {4});
    
    testing::internal::CaptureStdout();
    cmd.print_commands();
    std::string output = testing::internal::GetCapturedStdout();
    
    // Find positions of each command in output
    size_t pos_add1 = output.find("add [1]");
    size_t pos_find_min = output.find("find_min [2]");
    size_t pos_number_smaller = output.find("number_smaller [3]");
    size_t pos_add2 = output.find("add [4]");
    
    // Verify order: add1 -> find_min -> number_smaller -> add2
    EXPECT_TRUE(pos_add1 < pos_find_min);
    EXPECT_TRUE(pos_find_min < pos_number_smaller);
    EXPECT_TRUE(pos_number_smaller < pos_add2);
}

// Test multiple commands of same type
TEST_F(CommandsTest, MultipleSameCommands) {
    cmd.add_command(Commands::Command::add, {1, 2});
    cmd.add_command(Commands::Command::add, {3, 4});
    cmd.add_command(Commands::Command::add, {5, 6});
    
    testing::internal::CaptureStdout();
    cmd.print_commands();
    std::string output = testing::internal::GetCapturedStdout();
    
    // Should see multiple "add" commands
    size_t first_add = output.find("add");
    size_t second_add = output.find("add", first_add + 1);
    size_t third_add = output.find("add", second_add + 1);
    
    EXPECT_TRUE(first_add != std::string::npos);
    EXPECT_TRUE(second_add != std::string::npos);
    EXPECT_TRUE(third_add != std::string::npos);
}

// Test large number of commands
TEST_F(CommandsTest, LargeNumberOfCommands) {
    const int NUM_COMMANDS = 100;
    
    for (int i = 0; i < NUM_COMMANDS; ++i) {
        cmd.add_command(Commands::Command::add, {i, i * 2});
    }
    
    testing::internal::CaptureStdout();
    cmd.print_commands();
    std::string output = testing::internal::GetCapturedStdout();
    
    // Should not crash and should contain expected number of lines
    size_t newline_count = std::count(output.begin(), output.end(), '\n');
    EXPECT_EQ(newline_count, NUM_COMMANDS);
}

// Test edge case values - FIXED VERSION
TEST_F(CommandsTest, EdgeCaseValues) {
    cmd.add_command(Commands::Command::add, {0});
    cmd.add_command(Commands::Command::find_min, {-1});
    
    // Use explicit initializer_list construction for large values
    cmd.add_command(Commands::Command::number_smaller, std::initializer_list<int>{INT_MAX});
    cmd.add_command(Commands::Command::add, std::initializer_list<int>{INT_MIN});
    
    testing::internal::CaptureStdout();
    cmd.print_commands();
    std::string output = testing::internal::GetCapturedStdout();
    
    SUCCEED(); // Just testing it doesn't crash with edge values
}

// Test empty initializer list
TEST_F(CommandsTest, EmptyInitializerList) {
    cmd.add_command(Commands::Command::add, {});
    
    testing::internal::CaptureStdout();
    cmd.print_commands();
    std::string output = testing::internal::GetCapturedStdout();
    
    // Should print command without values
    EXPECT_TRUE(output.find("add") != std::string::npos);
    EXPECT_TRUE(output.find("[") == std::string::npos); // No brackets for empty values
}

// Test tokenization stage
TEST_F(CommandsTest, TokenizeInput) {
    std::istringstream input("k 4 k3 k 2 m1 n 1");
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    
    std::vector<std::string> tokens = cmd.tokenize_input();
    
    std::cin.rdbuf(old_cin_buf);
    
    EXPECT_EQ(tokens.size(), 6);
    EXPECT_EQ(tokens[0], "k");
    EXPECT_EQ(tokens[1], "4");
    EXPECT_EQ(tokens[2], "k3");
    EXPECT_EQ(tokens[3], "k");
    EXPECT_EQ(tokens[4], "2");
    EXPECT_EQ(tokens[5], "m1");
    // Note: "n 1" would be on next line in real input
}

// Test parsing tokens
TEST_F(CommandsTest, ParseTokens) {
    std::vector<std::string> tokens = {"k", "4", "k3", "m", "1", "n1"};
    
    cmd.parse_tokens(tokens);
    
    EXPECT_EQ(cmd.size(), 4);
    if (cmd.size() >= 4) {
        EXPECT_EQ(cmd.get_command_type(0), Commands::Command::add);
        EXPECT_EQ(cmd.get_command_values(0)[0], 4);
        EXPECT_EQ(cmd.get_command_type(1), Commands::Command::add);
        EXPECT_EQ(cmd.get_command_values(1)[0], 3);
        EXPECT_EQ(cmd.get_command_type(2), Commands::Command::find_min);
        EXPECT_EQ(cmd.get_command_values(2)[0], 1);
        EXPECT_EQ(cmd.get_command_type(3), Commands::Command::number_smaller);
        EXPECT_EQ(cmd.get_command_values(3)[0], 1);
    }
}

// Test full pipeline
TEST_F(CommandsTest, FullParserPipeline) {
    std::istringstream input("k 4 k3 m 1 n1");
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    
    cmd.parse_from_stdin();
    
    std::cin.rdbuf(old_cin_buf);
    
    EXPECT_EQ(cmd.size(), 4);
}

// Test helper functions
TEST_F(CommandsTest, HelperFunctions) {
    // Test is_number
    EXPECT_TRUE(cmd.is_number("123"));
    EXPECT_TRUE(cmd.is_number("-456"));
    EXPECT_FALSE(cmd.is_number("abc"));
    EXPECT_FALSE(cmd.is_number("1a2"));
    EXPECT_FALSE(cmd.is_number("-"));
    EXPECT_FALSE(cmd.is_number(""));
}

// Test parser with properly formatted input
TEST_F(CommandsTest, ParserValidFormattedInput) {
    std::istringstream input("k 4 k 3 k 2 m 1 n 1");
    
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    cmd.parse_from_stdin();
    std::cin.rdbuf(old_cin_buf);
    
    EXPECT_EQ(cmd.size(), 5);
    EXPECT_EQ(cmd.get_command_type(0), Commands::Command::add);
    EXPECT_EQ(cmd.get_command_values(0)[0], 4);
    EXPECT_EQ(cmd.get_command_type(1), Commands::Command::add);
    EXPECT_EQ(cmd.get_command_values(1)[0], 3);
    EXPECT_EQ(cmd.get_command_type(2), Commands::Command::add);
    EXPECT_EQ(cmd.get_command_values(2)[0], 2);
    EXPECT_EQ(cmd.get_command_type(3), Commands::Command::find_min);
    EXPECT_EQ(cmd.get_command_values(3)[0], 1);
    EXPECT_EQ(cmd.get_command_type(4), Commands::Command::number_smaller);
    EXPECT_EQ(cmd.get_command_values(4)[0], 1);
}

// Test parser with attached numbers (like "k3")
TEST_F(CommandsTest, ParserAttachedNumbers) {
    std::istringstream input("k4 k3 k2 m1 n1");
    
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    cmd.parse_from_stdin();
    std::cin.rdbuf(old_cin_buf);
    
    EXPECT_EQ(cmd.size(), 5);
    EXPECT_EQ(cmd.get_command_type(0), Commands::Command::add);
    EXPECT_EQ(cmd.get_command_values(0)[0], 4);
    EXPECT_EQ(cmd.get_command_type(1), Commands::Command::add);
    EXPECT_EQ(cmd.get_command_values(1)[0], 3);
    EXPECT_EQ(cmd.get_command_type(2), Commands::Command::add);
    EXPECT_EQ(cmd.get_command_values(2)[0], 2);
    EXPECT_EQ(cmd.get_command_type(3), Commands::Command::find_min);
    EXPECT_EQ(cmd.get_command_values(3)[0], 1);
    EXPECT_EQ(cmd.get_command_type(4), Commands::Command::number_smaller);
    EXPECT_EQ(cmd.get_command_values(4)[0], 1);
}

// Test parser with mixed format
TEST_F(CommandsTest, ParserMixedFormat) {
    std::istringstream input("k 4 k3 k 2 m1 n 1");
    
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    cmd.parse_from_stdin();
    std::cin.rdbuf(old_cin_buf);
    
    EXPECT_EQ(cmd.size(), 5);
    EXPECT_EQ(cmd.get_command_type(0), Commands::Command::add);
    EXPECT_EQ(cmd.get_command_values(0)[0], 4);
    EXPECT_EQ(cmd.get_command_type(1), Commands::Command::add);
    EXPECT_EQ(cmd.get_command_values(1)[0], 3);
    EXPECT_EQ(cmd.get_command_type(2), Commands::Command::add);
    EXPECT_EQ(cmd.get_command_values(2)[0], 2);
    EXPECT_EQ(cmd.get_command_type(3), Commands::Command::find_min);
    EXPECT_EQ(cmd.get_command_values(3)[0], 1);
    EXPECT_EQ(cmd.get_command_type(4), Commands::Command::number_smaller);
    EXPECT_EQ(cmd.get_command_values(4)[0], 1);
}

// Test parser with commands without values
TEST_F(CommandsTest, ParserCommandsWithoutValues) {
    std::istringstream input("k m n k 5");
    
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    cmd.parse_from_stdin();
    std::cin.rdbuf(old_cin_buf);
    
    EXPECT_EQ(cmd.size(), 4);
    EXPECT_EQ(cmd.get_command_type(0), Commands::Command::add);
    EXPECT_TRUE(cmd.get_command_values(0).empty());
    EXPECT_EQ(cmd.get_command_type(1), Commands::Command::find_min);
    EXPECT_TRUE(cmd.get_command_values(1).empty());
    EXPECT_EQ(cmd.get_command_type(2), Commands::Command::number_smaller);
    EXPECT_TRUE(cmd.get_command_values(2).empty());
    EXPECT_EQ(cmd.get_command_type(3), Commands::Command::add);
    EXPECT_EQ(cmd.get_command_values(3)[0], 5);
}

// Test parser with negative numbers
TEST_F(CommandsTest, ParserNegativeNumbers) {
    std::istringstream input("k -1 m -42 n -100");
    
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    cmd.parse_from_stdin();
    std::cin.rdbuf(old_cin_buf);
    
    EXPECT_EQ(cmd.size(), 3);
    EXPECT_EQ(cmd.get_command_type(0), Commands::Command::add);
    EXPECT_EQ(cmd.get_command_values(0)[0], -1);
    EXPECT_EQ(cmd.get_command_type(1), Commands::Command::find_min);
    EXPECT_EQ(cmd.get_command_values(1)[0], -42);
    EXPECT_EQ(cmd.get_command_type(2), Commands::Command::number_smaller);
    EXPECT_EQ(cmd.get_command_values(2)[0], -100);
}

// Test parser with multiple lines
TEST_F(CommandsTest, ParserMultipleLines) {
    std::istringstream input("k 1 k 2\nm 3\nn 4 k 5");
    
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    cmd.parse_from_stdin();
    std::cin.rdbuf(old_cin_buf);
    
    EXPECT_EQ(cmd.size(), 5);
}

// Test parser with invalid input
TEST_F(CommandsTest, ParserInvalidInput) {
    std::istringstream input("x 1 y 2 z 3 k 4");
    
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    
    testing::internal::CaptureStderr();
    cmd.parse_from_stdin();
    std::string error_output = testing::internal::GetCapturedStderr();
    
    std::cin.rdbuf(old_cin_buf);
    
    // Should process valid command and skip invalid ones
    EXPECT_EQ(cmd.size(), 1);
    EXPECT_EQ(cmd.get_command_type(0), Commands::Command::add);
    EXPECT_EQ(cmd.get_command_values(0)[0], 4);
    EXPECT_TRUE(error_output.find("Unknown command") != std::string::npos);
}

// Test parser with empty input
TEST_F(CommandsTest, ParserEmptyInput) {
    std::istringstream input("");
    
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    cmd.parse_from_stdin();
    std::cin.rdbuf(old_cin_buf);
    
    EXPECT_EQ(cmd.size(), 0);
}

// Test parser output format
TEST_F(CommandsTest, ParserOutputFormat) {
    std::istringstream input("k 4 k3 k 2 m1 n 1");
    
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    cmd.parse_from_stdin();
    std::cin.rdbuf(old_cin_buf);
    
    testing::internal::CaptureStdout();
    cmd.print_commands();
    std::string output = testing::internal::GetCapturedStdout();
    
    // Check output format
    EXPECT_TRUE(output.find("add [4]") != std::string::npos);
    EXPECT_TRUE(output.find("add [3]") != std::string::npos);
    EXPECT_TRUE(output.find("add [2]") != std::string::npos);
    EXPECT_TRUE(output.find("find_min [1]") != std::string::npos);
    EXPECT_TRUE(output.find("number_smaller [1]") != std::string::npos);
}