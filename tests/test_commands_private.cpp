#include <gtest/gtest.h>
#include "Commands.hpp"
#include <sstream>

// Tests for PRIVATE implementation - white box testing
class CommandsTest_PrivateImplementation : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}

    Commands cmd;
};

// Tokenization tests
TEST_F(CommandsTest_PrivateImplementation, TokenizeInput) {
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
}

TEST_F(CommandsTest_PrivateImplementation, TokenizeEmptyInput) {
    std::istringstream input("");
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    
    std::vector<std::string> tokens = cmd.tokenize_input();
    std::cin.rdbuf(old_cin_buf);
    
    EXPECT_TRUE(tokens.empty());
}

TEST_F(CommandsTest_PrivateImplementation, TokenizeMultipleLines) {
    std::istringstream input("k 1 k 2\nm 3\nn 4");
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    
    std::vector<std::string> tokens = cmd.tokenize_input();
    std::cin.rdbuf(old_cin_buf);
    
    EXPECT_EQ(tokens.size(), 6);
}

// Token parsing tests
TEST_F(CommandsTest_PrivateImplementation, ParseTokens) {
    std::vector<std::string> tokens = {"k", "4", "k3", "m", "1", "n1"};
    cmd.parse_tokens(tokens);
    
    EXPECT_EQ(cmd.size(), 4);
    EXPECT_EQ(cmd.get_command_type(0), Commands::Command::add);
    EXPECT_EQ(cmd.get_command_values(0)[0], 4);
    EXPECT_EQ(cmd.get_command_type(1), Commands::Command::add);
    EXPECT_EQ(cmd.get_command_values(1)[0], 3);
    EXPECT_EQ(cmd.get_command_type(2), Commands::Command::find_min);
    EXPECT_EQ(cmd.get_command_values(2)[0], 1);
    EXPECT_EQ(cmd.get_command_type(3), Commands::Command::number_smaller);
    EXPECT_EQ(cmd.get_command_values(3)[0], 1);
}

TEST_F(CommandsTest_PrivateImplementation, ParseTokensCommandsWithoutValues) {
    std::vector<std::string> tokens = {"k", "m", "n"};
    cmd.parse_tokens(tokens);
    
    EXPECT_EQ(cmd.size(), 3);
    EXPECT_TRUE(cmd.get_command_values(0).empty());
    EXPECT_TRUE(cmd.get_command_values(1).empty());
    EXPECT_TRUE(cmd.get_command_values(2).empty());
}

TEST_F(CommandsTest_PrivateImplementation, ParseTokensInvalidTokens) {
    std::vector<std::string> tokens = {"x", "k", "abc", "m", "5"};
    
    testing::internal::CaptureStderr();
    cmd.parse_tokens(tokens);
    std::string error_output = testing::internal::GetCapturedStderr();
    
    EXPECT_EQ(cmd.size(), 2);
    EXPECT_TRUE(error_output.find("Unknown token") != std::string::npos);
    EXPECT_TRUE(error_output.find("Invalid number") != std::string::npos);
}

// Helper function tests
TEST_F(CommandsTest_PrivateImplementation, IsNumber) {
    EXPECT_TRUE(cmd.is_number("123"));
    EXPECT_TRUE(cmd.is_number("-456"));
    EXPECT_TRUE(cmd.is_number("0"));
    EXPECT_FALSE(cmd.is_number("abc"));
    EXPECT_FALSE(cmd.is_number("1a2"));
    EXPECT_FALSE(cmd.is_number("-"));
    EXPECT_FALSE(cmd.is_number(""));
    EXPECT_FALSE(cmd.is_number("12.3"));
}

TEST_F(CommandsTest_PrivateImplementation, CharToCommand) {
    EXPECT_EQ(cmd.char_to_command('k'), Commands::Command::add);
    EXPECT_EQ(cmd.char_to_command('m'), Commands::Command::find_min);
    EXPECT_EQ(cmd.char_to_command('n'), Commands::Command::number_smaller);
    // Default case for unknown characters
    EXPECT_EQ(cmd.char_to_command('x'), Commands::Command::add);
}

// Edge case parsing tests
TEST_F(CommandsTest_PrivateImplementation, ParseTokensEdgeCases) {
    // Test attached numbers with various formats
    std::vector<std::string> tokens = {"k4", "m-5", "n123", "k0"};
    cmd.parse_tokens(tokens);
    
    EXPECT_EQ(cmd.size(), 4);
    EXPECT_EQ(cmd.get_command_values(0)[0], 4);
    EXPECT_EQ(cmd.get_command_values(1)[0], -5);
    EXPECT_EQ(cmd.get_command_values(2)[0], 123);
    EXPECT_EQ(cmd.get_command_values(3)[0], 0);
}

TEST_F(CommandsTest_PrivateImplementation, ParseTokensInvalidAttachedNumbers) {
    std::vector<std::string> tokens = {"k4a", "m-", "n12x3"};
    
    testing::internal::CaptureStderr();
    cmd.parse_tokens(tokens);
    std::string error_output = testing::internal::GetCapturedStderr();
    
    // Should handle gracefully and potentially add commands without values
    EXPECT_TRUE(error_output.find("Invalid number") != std::string::npos);
}