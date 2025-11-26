#include <gtest/gtest.h>
#include <sstream>
#include "AVLTreeIntegration.hpp"

class AVLTreeIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        integration.clear();
    }

    AVLTreeIntegration integration;
};

TEST_F(AVLTreeIntegrationTest, ErrorHandling_ValidCommandsStillExecute) {
    std::istringstream input("invalid\nk 5 10\nm 2");
    std::ostringstream output;
    std::ostringstream errors;
    
    int result = integration.executeFromStream(input, output, errors);
    
    EXPECT_GT(result, 0);  // Has parsing error for "invalid"
    EXPECT_EQ(output.str(), "10\n");  // 2nd smallest of [5,10] is 10
    EXPECT_TRUE(errors.str().find("Unknown token: invalid") != std::string::npos);
}

TEST_F(AVLTreeIntegrationTest, ErrorHandling_CommandWithoutValues) {
    std::istringstream input("m");
    std::ostringstream output;
    std::ostringstream errors;
    
    int result = integration.executeFromStream(input, output, errors);
    
    EXPECT_GT(result, 0);  // Has execution error!
    EXPECT_EQ(output.str(), "\n");  // But still outputs empty line
    EXPECT_TRUE(errors.str().find("find_min requires exactly one value, got 0") != std::string::npos);
}

TEST_F(AVLTreeIntegrationTest, ErrorHandling_MixedValidAndInvalid) {
    std::istringstream input("k 5\ninvalid\nm 1");
    std::ostringstream output;
    std::ostringstream errors;
    
    int result = integration.executeFromStream(input, output, errors);
    
    EXPECT_GT(result, 0);  // Has parsing error for "invalid"
    EXPECT_EQ(output.str(), "5\n");  // 1st smallest of [5] is 5
    EXPECT_TRUE(errors.str().find("Unknown token: invalid") != std::string::npos);
}

TEST_F(AVLTreeIntegrationTest, EmptyAddCommand) {
    std::istringstream input("k");
    std::ostringstream output;
    std::ostringstream errors;
    
    int result = integration.executeFromStream(input, output, errors);
    
    EXPECT_GT(result, 0);  // Now has execution error!
    EXPECT_TRUE(output.str().empty());  // No output for empty add
    EXPECT_TRUE(errors.str().find("add requires at least one value, got 0") != std::string::npos);
}

TEST_F(AVLTreeIntegrationTest, EmptyNumberSmallerCommand) {
    std::istringstream input("n");
    std::ostringstream output;
    std::ostringstream errors;
    
    int result = integration.executeFromStream(input, output, errors);
    
    EXPECT_GT(result, 0);  // Has execution error!
    EXPECT_EQ(output.str(), "0\n");  // But still outputs 0
    EXPECT_TRUE(errors.str().find("number_smaller requires exactly one value, got 0") != std::string::npos);
}

TEST_F(AVLTreeIntegrationTest, MultipleValuesForFindMin) {
    std::istringstream input("k 5 10 15\nm 2 99");  // m has extra values
    std::ostringstream output;
    std::ostringstream errors;
    
    int result = integration.executeFromStream(input, output, errors);
    
    EXPECT_GT(result, 0);  // Has execution error!
    EXPECT_EQ(output.str(), "\n");  // No output the result
    EXPECT_TRUE(errors.str().find("find_min requires exactly one value, got 2") != std::string::npos);
}


TEST_F(AVLTreeIntegrationTest, ValidAddCommand) {
    std::istringstream input("k 5 10 15");
    std::ostringstream output;
    std::ostringstream errors;
    
    int result = integration.executeFromStream(input, output, errors);
    
    EXPECT_EQ(result, 0);  // No errors
    EXPECT_TRUE(output.str().empty());  // No output for add
    EXPECT_TRUE(errors.str().empty());  // No errors
}



TEST_F(AVLTreeIntegrationTest, MultipleValuesForNumberSmaller) {
    std::istringstream input("k 5 10 15\nn 10 99");  // n has extra values - should error
    std::ostringstream output;
    std::ostringstream errors;
    
    int result = integration.executeFromStream(input, output, errors);
    
    EXPECT_GT(result, 0);  // Should have execution error
    EXPECT_EQ(output.str(), "0\n");  // 0 due to error (not "2\n")
    EXPECT_TRUE(errors.str().find("number_smaller requires exactly one value") != std::string::npos);
}

// Add more tests for valid scenarios to replace the "bad command" tests
TEST_F(AVLTreeIntegrationTest, ValidMultipleAddCommands) {
    std::istringstream input("k 5 10\nk 15 20\nm 3\nn 18");
    std::ostringstream output;
    std::ostringstream errors;
    
    int result = integration.executeFromStream(input, output, errors);
    
    EXPECT_EQ(result, 0);
    EXPECT_EQ(output.str(), "15\n3\n"); // 3rd smallest is 15, 3 elements smaller than 18
    EXPECT_TRUE(errors.str().empty());
}

TEST_F(AVLTreeIntegrationTest, ValidComplexScenario) {
    std::istringstream input("k 50 30 70 20 40 60 80\nm 1\nm 4\nm 7\nn 25\nn 55\nn 100");
    std::ostringstream output;
    std::ostringstream errors;
    
    int result = integration.executeFromStream(input, output, errors);
    
    EXPECT_EQ(result, 0);
    // Tree: [20, 30, 40, 50, 60, 70, 80]
    // m 1 -> 1st smallest = 20
    // m 4 -> 4th smallest = 50  
    // m 7 -> 7th smallest = 80
    // n 25 -> elements smaller than 25: only 20 = 1 element
    // n 55 -> elements smaller than 55: 20, 30, 40, 50 = 4 elements
    // n 100 -> elements smaller than 100: all 7 elements
    EXPECT_EQ(output.str(), "20\n50\n80\n1\n4\n7\n");
    EXPECT_TRUE(errors.str().empty());
}


TEST_F(AVLTreeIntegrationTest, ValidNegativeNumbers) {
    std::istringstream input("k -5 0 5\nm 2\nn -3\nn 3");
    std::ostringstream output;
    std::ostringstream errors;
    
    int result = integration.executeFromStream(input, output, errors);
    
    EXPECT_EQ(result, 0);
    EXPECT_EQ(output.str(), "0\n1\n2\n"); // 2nd smallest is 0, 1 element < -3, 2 elements < 3
    EXPECT_TRUE(errors.str().empty());
}

TEST_F(AVLTreeIntegrationTest, ValidEmptyTreeQueries) {
    std::istringstream input("m 1\nn 5");
    std::ostringstream output;
    std::ostringstream errors;
    
    int result = integration.executeFromStream(input, output, errors);
    
    EXPECT_EQ(result, 0);
    EXPECT_EQ(output.str(), "\n0\n"); // Empty line for find_min, 0 for number_smaller
    EXPECT_TRUE(errors.str().empty());
}

TEST_F(AVLTreeIntegrationTest, ValidFindMinOutOfBounds) {
    std::istringstream input("k 5 10\nm 5"); // Only 2 elements, asking for 5th
    std::ostringstream output;
    std::ostringstream errors;
    
    int result = integration.executeFromStream(input, output, errors);
    
    EXPECT_EQ(result, 0);
    EXPECT_EQ(output.str(), "\n"); // Empty line for no result
    EXPECT_TRUE(errors.str().empty());
}
