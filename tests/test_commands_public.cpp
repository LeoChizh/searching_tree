#include <gtest/gtest.h>
#include "Commands.hpp"
#include <climits>
#include <algorithm>
#include <sstream>

// Tests for PUBLIC interface only - black box testing
class CommandsTest_PublicInterface : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}

    Commands cmd;
};

// Basic functionality tests
TEST_F(CommandsTest_PublicInterface, DefaultConstruction) {
    SUCCEED();
}

TEST_F(CommandsTest_PublicInterface, AddCommandNoValues) {
    cmd.add_command(Commands::Command::add);
    cmd.add_command(Commands::Command::find_min);
    cmd.add_command(Commands::Command::number_smaller);
    SUCCEED();
}

TEST_F(CommandsTest_PublicInterface, AddCommandSingleValue) {
    cmd.add_command(Commands::Command::add, {42});
    cmd.add_command(Commands::Command::find_min, {100});
    cmd.add_command(Commands::Command::number_smaller, {7});
    SUCCEED();
}

TEST_F(CommandsTest_PublicInterface, AddCommandMultipleValues) {
    cmd.add_command(Commands::Command::add, {10, 20});
    cmd.add_command(Commands::Command::number_smaller, {1, 2, 3, 4, 5});
    SUCCEED();
}

TEST_F(CommandsTest_PublicInterface, AddMixedCommands) {
    cmd.add_command(Commands::Command::add);
    cmd.add_command(Commands::Command::find_min, {5});
    cmd.add_command(Commands::Command::number_smaller, {1, 2, 3});
    cmd.add_command(Commands::Command::add, {100, 200});
    SUCCEED();
}

// Output tests
TEST_F(CommandsTest_PublicInterface, PrintEmptyCommands) {
    testing::internal::CaptureStdout();
    cmd.print_commands();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.empty());
}

TEST_F(CommandsTest_PublicInterface, PrintCommandsOutput) {
    cmd.add_command(Commands::Command::add, {10, 20});
    cmd.add_command(Commands::Command::find_min, {5});
    cmd.add_command(Commands::Command::number_smaller);
    cmd.add_command(Commands::Command::number_smaller, {1, 2, 3});
    
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

TEST_F(CommandsTest_PublicInterface, CommandOrderPreserved) {
    cmd.add_command(Commands::Command::add, {1});
    cmd.add_command(Commands::Command::find_min, {2});
    cmd.add_command(Commands::Command::number_smaller, {3});
    cmd.add_command(Commands::Command::add, {4});
    
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
TEST_F(CommandsTest_PublicInterface, EdgeCaseValues) {
    cmd.add_command(Commands::Command::add, {0});
    cmd.add_command(Commands::Command::find_min, {-1});
    cmd.add_command(Commands::Command::number_smaller, std::initializer_list<int>{INT_MAX});
    cmd.add_command(Commands::Command::add, std::initializer_list<int>{INT_MIN});
    
    testing::internal::CaptureStdout();
    cmd.print_commands();
    std::string output = testing::internal::GetCapturedStdout();
    SUCCEED();
}

// Parser tests through public interface
TEST_F(CommandsTest_PublicInterface, ParserValidFormattedInput) {
    std::istringstream input("k 4 k 3 k 2 m 1 n 1");
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    cmd.parse_from_stdin();
    std::cin.rdbuf(old_cin_buf);
    
    EXPECT_EQ(cmd.size(), 5);
    EXPECT_EQ(cmd.get_command_type(0), Commands::Command::add);
    EXPECT_EQ(cmd.get_command_values(0)[0], 4);
}

TEST_F(CommandsTest_PublicInterface, ParserAttachedNumbers) {
    std::istringstream input("k4 k3 k2 m1 n1");
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    cmd.parse_from_stdin();
    std::cin.rdbuf(old_cin_buf);
    
    EXPECT_EQ(cmd.size(), 5);
    EXPECT_EQ(cmd.get_command_type(0), Commands::Command::add);
    EXPECT_EQ(cmd.get_command_values(0)[0], 4);
}

TEST_F(CommandsTest_PublicInterface, ParserMixedFormat) {
    std::istringstream input("k 4 k3 k 2 m1 n 1");
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    cmd.parse_from_stdin();
    std::cin.rdbuf(old_cin_buf);
    
    EXPECT_EQ(cmd.size(), 5);
}

TEST_F(CommandsTest_PublicInterface, ParserCommandsWithoutValues) {
    std::istringstream input("k m n k 5");
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    cmd.parse_from_stdin();
    std::cin.rdbuf(old_cin_buf);
    
    EXPECT_EQ(cmd.size(), 4);
    EXPECT_TRUE(cmd.get_command_values(0).empty());
    EXPECT_EQ(cmd.get_command_values(3)[0], 5);
}

TEST_F(CommandsTest_PublicInterface, ParserInvalidInput) {
    std::istringstream input("x 1 y 2 z 3 k 4");
    auto old_cin_buf = std::cin.rdbuf(input.rdbuf());
    
    testing::internal::CaptureStderr();
    cmd.parse_from_stdin();
    std::string error_output = testing::internal::GetCapturedStderr();
    std::cin.rdbuf(old_cin_buf);
    
    EXPECT_EQ(cmd.size(), 1);
    EXPECT_TRUE(error_output.find("Unknown token") != std::string::npos);
}