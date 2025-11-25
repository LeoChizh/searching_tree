#include <gtest/gtest.h>
#include "AVLTree.hpp"

class AVLTreeNthSmallestTest : public ::testing::Test {
protected:
    void SetUp() override {
        tree = std::make_unique<AVLTree>();
    }

    void TearDown() override {
        tree.reset();
    }

    std::unique_ptr<AVLTree> tree;
};

TEST_F(AVLTreeNthSmallestTest, EmptyTree) {
    EXPECT_FALSE(tree->findNthSmallest(1).has_value());
    EXPECT_FALSE(tree->findNthSmallest(0).has_value());
    EXPECT_FALSE(tree->findNthSmallest(-1).has_value());
    EXPECT_FALSE(tree->findNthSmallest(100).has_value());
}

TEST_F(AVLTreeNthSmallestTest, SingleElement) {
    tree->insert(42);
    
    EXPECT_FALSE(tree->findNthSmallest(0).has_value()); // n=0 invalid
    EXPECT_EQ(tree->findNthSmallest(1).value(), 42);    // 1st smallest
    EXPECT_FALSE(tree->findNthSmallest(2).has_value()); // n=2 out of bounds
    EXPECT_FALSE(tree->findNthSmallest(-1).has_value());// negative n
}

TEST_F(AVLTreeNthSmallestTest, TwoElements) {
    tree->insert(30);
    tree->insert(20);
    
    // Tree: 20, 30
    EXPECT_EQ(tree->findNthSmallest(1).value(), 20); // 1st smallest
    EXPECT_EQ(tree->findNthSmallest(2).value(), 30); // 2nd smallest
    EXPECT_FALSE(tree->findNthSmallest(3).has_value()); // out of bounds
    EXPECT_FALSE(tree->findNthSmallest(0).has_value()); // invalid
}

TEST_F(AVLTreeNthSmallestTest, ThreeElements) {
    tree->insert(50);
    tree->insert(30);
    tree->insert(70);
    
    // Sorted: 30, 50, 70
    EXPECT_EQ(tree->findNthSmallest(1).value(), 30);
    EXPECT_EQ(tree->findNthSmallest(2).value(), 50);
    EXPECT_EQ(tree->findNthSmallest(3).value(), 70);
    EXPECT_FALSE(tree->findNthSmallest(4).has_value());
}

TEST_F(AVLTreeNthSmallestTest, SevenElements) {
    tree->insert(50);
    tree->insert(30);
    tree->insert(70);
    tree->insert(20);
    tree->insert(40);
    tree->insert(60);
    tree->insert(80);
    
    // Sorted: 20, 30, 40, 50, 60, 70, 80
    EXPECT_EQ(tree->findNthSmallest(1).value(), 20);
    EXPECT_EQ(tree->findNthSmallest(2).value(), 30);
    EXPECT_EQ(tree->findNthSmallest(3).value(), 40);
    EXPECT_EQ(tree->findNthSmallest(4).value(), 50);
    EXPECT_EQ(tree->findNthSmallest(5).value(), 60);
    EXPECT_EQ(tree->findNthSmallest(6).value(), 70);
    EXPECT_EQ(tree->findNthSmallest(7).value(), 80);
    EXPECT_FALSE(tree->findNthSmallest(8).has_value());
    EXPECT_FALSE(tree->findNthSmallest(0).has_value());
}

TEST_F(AVLTreeNthSmallestTest, WithDuplicatesPrevention) {
    tree->insert(50);
    tree->insert(30);
    tree->insert(70);
    tree->insert(30); // Duplicate - should be ignored
    tree->insert(50); // Duplicate - should be ignored
    
    // Sorted: 30, 50, 70 (duplicates not inserted)
    EXPECT_EQ(tree->findNthSmallest(1).value(), 30);
    EXPECT_EQ(tree->findNthSmallest(2).value(), 50);
    EXPECT_EQ(tree->findNthSmallest(3).value(), 70);
    EXPECT_FALSE(tree->findNthSmallest(4).has_value());
}

TEST_F(AVLTreeNthSmallestTest, AfterRebalancing) {
    // Insert to cause rebalancing
    tree->insert(10);
    tree->insert(20);
    tree->insert(30); // Should trigger left rotation
    
    // After rebalancing, sorted order should still be: 10, 20, 30
    EXPECT_EQ(tree->findNthSmallest(1).value(), 10);
    EXPECT_EQ(tree->findNthSmallest(2).value(), 20);
    EXPECT_EQ(tree->findNthSmallest(3).value(), 30);
}

TEST_F(AVLTreeNthSmallestTest, ComplexBalancedTree) {
    // Build a more complex balanced tree
    tree->insert(50);
    tree->insert(25);
    tree->insert(75);
    tree->insert(10);
    tree->insert(30);
    tree->insert(60);
    tree->insert(80);
    tree->insert(5);
    tree->insert(15);
    tree->insert(27);
    tree->insert(55);
    
    // Sorted: 5, 10, 15, 25, 27, 30, 50, 55, 60, 75, 80
    EXPECT_EQ(tree->findNthSmallest(1).value(), 5);
    EXPECT_EQ(tree->findNthSmallest(2).value(), 10);
    EXPECT_EQ(tree->findNthSmallest(3).value(), 15);
    EXPECT_EQ(tree->findNthSmallest(4).value(), 25);
    EXPECT_EQ(tree->findNthSmallest(5).value(), 27);
    EXPECT_EQ(tree->findNthSmallest(6).value(), 30);
    EXPECT_EQ(tree->findNthSmallest(7).value(), 50);
    EXPECT_EQ(tree->findNthSmallest(8).value(), 55);
    EXPECT_EQ(tree->findNthSmallest(9).value(), 60);
    EXPECT_EQ(tree->findNthSmallest(10).value(), 75);
    EXPECT_EQ(tree->findNthSmallest(11).value(), 80);
    EXPECT_FALSE(tree->findNthSmallest(12).has_value());
}

TEST_F(AVLTreeNthSmallestTest, AfterRemovals) {
    tree->insert(50);
    tree->insert(30);
    tree->insert(70);
    tree->insert(20);
    tree->insert(40);
    tree->insert(60);
    tree->insert(80);
    
    // Initial: 20, 30, 40, 50, 60, 70, 80
    EXPECT_EQ(tree->findNthSmallest(4).value(), 50);
    
    tree->remove(50); // Remove root
    // After removal: 20, 30, 40, 60, 70, 80
    EXPECT_EQ(tree->findNthSmallest(1).value(), 20);
    EXPECT_EQ(tree->findNthSmallest(2).value(), 30);
    EXPECT_EQ(tree->findNthSmallest(3).value(), 40);
    EXPECT_EQ(tree->findNthSmallest(4).value(), 60); // 4th smallest is now 60
    EXPECT_EQ(tree->findNthSmallest(5).value(), 70);
    EXPECT_EQ(tree->findNthSmallest(6).value(), 80);
    
    tree->remove(20); // Remove smallest
    // After removal: 30, 40, 60, 70, 80
    EXPECT_EQ(tree->findNthSmallest(1).value(), 30); // 1st smallest is now 30
    EXPECT_EQ(tree->findNthSmallest(2).value(), 40);
}

TEST_F(AVLTreeNthSmallestTest, NegativeAndZeroN) {
    tree->insert(10);
    tree->insert(20);
    tree->insert(30);
    
    EXPECT_FALSE(tree->findNthSmallest(0).has_value());  // n=0 invalid
    EXPECT_FALSE(tree->findNthSmallest(-1).has_value()); // negative n
    EXPECT_FALSE(tree->findNthSmallest(-5).has_value()); // negative n
}

TEST_F(AVLTreeNthSmallestTest, LargeN) {
    tree->insert(10);
    tree->insert(20);
    tree->insert(30);
    
    EXPECT_FALSE(tree->findNthSmallest(100).has_value());  // n much larger than size
    EXPECT_FALSE(tree->findNthSmallest(1000).has_value()); // n much larger than size
}

TEST_F(AVLTreeNthSmallestTest, SequentialCalls) {
    tree->insert(50);
    tree->insert(30);
    tree->insert(70);
    tree->insert(20);
    tree->insert(40);
    
    // Multiple sequential calls should work correctly
    EXPECT_EQ(tree->findNthSmallest(1).value(), 20);
    EXPECT_EQ(tree->findNthSmallest(2).value(), 30);
    EXPECT_EQ(tree->findNthSmallest(3).value(), 40);
    EXPECT_EQ(tree->findNthSmallest(4).value(), 50);
    EXPECT_EQ(tree->findNthSmallest(5).value(), 70);
    
    // Call again - should return same results
    EXPECT_EQ(tree->findNthSmallest(1).value(), 20);
    EXPECT_EQ(tree->findNthSmallest(5).value(), 70);
}

TEST_F(AVLTreeNthSmallestTest, MixedOperations) {
    // Test findNthSmallest with mixed insert/remove operations
    tree->insert(100);
    tree->insert(50);
    EXPECT_EQ(tree->findNthSmallest(1).value(), 50);
    EXPECT_EQ(tree->findNthSmallest(2).value(), 100);
    
    tree->insert(25);
    EXPECT_EQ(tree->findNthSmallest(1).value(), 25);
    EXPECT_EQ(tree->findNthSmallest(2).value(), 50);
    EXPECT_EQ(tree->findNthSmallest(3).value(), 100);
    
    tree->remove(50);
    EXPECT_EQ(tree->findNthSmallest(1).value(), 25);
    EXPECT_EQ(tree->findNthSmallest(2).value(), 100);
    EXPECT_FALSE(tree->findNthSmallest(3).has_value());
}