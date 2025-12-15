#include <gtest/gtest.h>
#include "AVLTree.hpp"

class AVLTreeTest : public ::testing::Test {
protected:
    // tree is stack-allocated, constructor called automatically
    AVLTree tree;
};

// Basic functionality tests
TEST_F(AVLTreeTest, DefaultConstructor) {
    EXPECT_TRUE(tree.empty());
    EXPECT_EQ(tree.size(), 0);
    EXPECT_EQ(tree.getHeight(), -1); // Empty tree height is -1
}

TEST_F(AVLTreeTest, InsertSingleValue) {
    bool result = tree.insert(42);
    EXPECT_TRUE(result);
    EXPECT_FALSE(tree.empty());
    EXPECT_EQ(tree.size(), 1);
    EXPECT_TRUE(tree.contains(42));
}

TEST_F(AVLTreeTest, InsertMultipleValues) {
    EXPECT_TRUE(tree.insert(10));
    EXPECT_TRUE(tree.insert(20));
    EXPECT_TRUE(tree.insert(5));
    
    EXPECT_EQ(tree.size(), 3);
    EXPECT_TRUE(tree.contains(10));
    EXPECT_TRUE(tree.contains(20));
    EXPECT_TRUE(tree.contains(5));
}

TEST_F(AVLTreeTest, InsertDuplicateValue) {
    EXPECT_TRUE(tree.insert(42));
    EXPECT_FALSE(tree.insert(42)); // Duplicate should fail
    EXPECT_EQ(tree.size(), 1);
}

TEST_F(AVLTreeTest, Contains) {
    EXPECT_FALSE(tree.contains(42));
    
    tree.insert(42);
    EXPECT_TRUE(tree.contains(42));
    EXPECT_FALSE(tree.contains(100));
}

TEST_F(AVLTreeTest, RemoveFromEmptyTree) {
    EXPECT_FALSE(tree.remove(42));
}

TEST_F(AVLTreeTest, RemoveNonExistentValue) {
    tree.insert(10);
    tree.insert(20);
    EXPECT_FALSE(tree.remove(42)); // Value not in tree
    EXPECT_EQ(tree.size(), 2);
}

TEST_F(AVLTreeTest, RemoveSingleNode) {
    tree.insert(42);
    EXPECT_TRUE(tree.remove(42));
    EXPECT_TRUE(tree.empty());
    EXPECT_FALSE(tree.contains(42));
}

TEST_F(AVLTreeTest, RemoveLeafNode) {
    tree.insert(10);
    tree.insert(5);
    tree.insert(15);
    
    EXPECT_TRUE(tree.remove(5)); // Remove left leaf
    EXPECT_EQ(tree.size(), 2);
    EXPECT_FALSE(tree.contains(5));
    EXPECT_TRUE(tree.contains(10));
    EXPECT_TRUE(tree.contains(15));
    
    EXPECT_TRUE(tree.remove(15)); // Remove right leaf
    EXPECT_EQ(tree.size(), 1);
    EXPECT_TRUE(tree.contains(10));
}

TEST_F(AVLTreeTest, RemoveNodeWithOneChild) {
    // Build: 10 -> 5 -> 2
    tree.insert(10);
    tree.insert(5);
    tree.insert(2);
    
    EXPECT_TRUE(tree.remove(5)); // Node with one child (2)
    EXPECT_EQ(tree.size(), 2);
    EXPECT_FALSE(tree.contains(5));
    EXPECT_TRUE(tree.contains(10));
    EXPECT_TRUE(tree.contains(2));
}

TEST_F(AVLTreeTest, RemoveNodeWithTwoChildren) {
    // Build: 10 -> (5, 15) -> (2, 7)
    tree.insert(10);
    tree.insert(5);
    tree.insert(15);
    tree.insert(2);
    tree.insert(7);
    
    EXPECT_TRUE(tree.remove(5)); // Node with two children
    EXPECT_EQ(tree.size(), 4);
    EXPECT_FALSE(tree.contains(5));
    // Tree should still contain all other values
    EXPECT_TRUE(tree.contains(10));
    EXPECT_TRUE(tree.contains(15));
    EXPECT_TRUE(tree.contains(2));
    EXPECT_TRUE(tree.contains(7));
}

TEST_F(AVLTreeTest, RemoveRootWithTwoChildren) {
    tree.insert(10);
    tree.insert(5);
    tree.insert(15);
    
    EXPECT_TRUE(tree.remove(10)); // Remove root with two children
    EXPECT_EQ(tree.size(), 2);
    EXPECT_FALSE(tree.contains(10));
    EXPECT_TRUE(tree.contains(5));
    EXPECT_TRUE(tree.contains(15));
}

// AVL balancing tests
TEST_F(AVLTreeTest, LeftLeftCase) {
    // Insert to cause left-left imbalance: 30, 20, 10
    tree.insert(30);
    tree.insert(20);
    tree.insert(10); // This should trigger right rotation
    
    EXPECT_EQ(tree.size(), 3);
    EXPECT_TRUE(tree.contains(10));
    EXPECT_TRUE(tree.contains(20));
    EXPECT_TRUE(tree.contains(30));
    // Tree should be balanced: 20 as root, 10 left, 30 right
}

TEST_F(AVLTreeTest, RightRightCase) {
    // Insert to cause right-right imbalance: 10, 20, 30
    tree.insert(10);
    tree.insert(20);
    tree.insert(30); // This should trigger left rotation
    
    EXPECT_EQ(tree.size(), 3);
    EXPECT_TRUE(tree.contains(10));
    EXPECT_TRUE(tree.contains(20));
    EXPECT_TRUE(tree.contains(30));
    // Tree should be balanced: 20 as root, 10 left, 30 right
}

TEST_F(AVLTreeTest, LeftRightCase) {
    // Insert to cause left-right imbalance: 30, 10, 20
    tree.insert(30);
    tree.insert(10);
    tree.insert(20); // This should trigger left-right rotation
    
    EXPECT_EQ(tree.size(), 3);
    EXPECT_TRUE(tree.contains(10));
    EXPECT_TRUE(tree.contains(20));
    EXPECT_TRUE(tree.contains(30));
}

TEST_F(AVLTreeTest, RightLeftCase) {
    // Insert to cause right-left imbalance: 10, 30, 20
    tree.insert(10);
    tree.insert(30);
    tree.insert(20); // This should trigger right-left rotation
    
    EXPECT_EQ(tree.size(), 3);
    EXPECT_TRUE(tree.contains(10));
    EXPECT_TRUE(tree.contains(20));
    EXPECT_TRUE(tree.contains(30));
}

TEST_F(AVLTreeTest, ComplexBalancing) {
    // Insert values that require multiple rotations
    tree.insert(50);
    tree.insert(25);
    tree.insert(75);
    tree.insert(10);
    tree.insert(30);
    tree.insert(60);
    tree.insert(80);
    tree.insert(5);
    tree.insert(15);
    tree.insert(27);
    tree.insert(55);
    
    EXPECT_EQ(tree.size(), 11);
    // All values should be present and tree should be balanced
    EXPECT_TRUE(tree.contains(50));
    EXPECT_TRUE(tree.contains(25));
    EXPECT_TRUE(tree.contains(75));
    EXPECT_TRUE(tree.contains(10));
    EXPECT_TRUE(tree.contains(30));
    EXPECT_TRUE(tree.contains(60));
    EXPECT_TRUE(tree.contains(80));
    EXPECT_TRUE(tree.contains(5));
    EXPECT_TRUE(tree.contains(15));
    EXPECT_TRUE(tree.contains(27));
    EXPECT_TRUE(tree.contains(55));
    
    // Tree should be balanced (height should be logarithmic)
    int height = tree.getHeight();
    EXPECT_LE(height, 4); // For 11 nodes, height should be <= 4 in AVL tree
}

// Copy and move semantics tests
TEST_F(AVLTreeTest, CopyConstructor) {
    tree.insert(10);
    tree.insert(20);
    tree.insert(5);
    
    AVLTree copy(tree); // Copy constructor
    
    EXPECT_EQ(copy.size(), 3);
    EXPECT_TRUE(copy.contains(10));
    EXPECT_TRUE(copy.contains(20));
    EXPECT_TRUE(copy.contains(5));
    
    // Modify original - copy should be unaffected
    tree.insert(15);
    EXPECT_EQ(tree.size(), 4);
    EXPECT_EQ(copy.size(), 3);
    EXPECT_FALSE(copy.contains(15));
}

TEST_F(AVLTreeTest, CopyAssignment) {
    tree.insert(10);
    tree.insert(20);
    
    AVLTree other;
    other = tree; // Copy assignment
    
    EXPECT_EQ(other.size(), 2);
    EXPECT_TRUE(other.contains(10));
    EXPECT_TRUE(other.contains(20));
    
    // Self-assignment should work
    other = other;
    EXPECT_EQ(other.size(), 2);
}

TEST_F(AVLTreeTest, MoveConstructor) {
    tree.insert(10);
    tree.insert(20);
    
    AVLTree moved(std::move(tree)); // Move constructor
    
    EXPECT_EQ(moved.size(), 2);
    EXPECT_TRUE(moved.contains(10));
    EXPECT_TRUE(moved.contains(20));
    
    // Original should be empty
    EXPECT_TRUE(tree.empty());
}

TEST_F(AVLTreeTest, MoveAssignment) {
    tree.insert(10);
    tree.insert(20);
    
    AVLTree other;
    other = std::move(tree); // Move assignment
    
    EXPECT_EQ(other.size(), 2);
    EXPECT_TRUE(other.contains(10));
    EXPECT_TRUE(other.contains(20));
    
    // Original should be empty
    EXPECT_TRUE(tree.empty());
}

TEST_F(AVLTreeTest, ClearNonEmptyTree) {
    tree.insert(10);
    tree.insert(20);
    tree.insert(5);
    
    tree = AVLTree();
    EXPECT_TRUE(tree.empty());
    EXPECT_EQ(tree.size(), 0);
    EXPECT_FALSE(tree.contains(10));
    EXPECT_FALSE(tree.contains(20));
    EXPECT_FALSE(tree.contains(5));
}

TEST_F(AVLTreeTest, ClearAndReuse) {
    tree.insert(10);
    tree.insert(20);
    tree = AVLTree();
    
    // Should be able to reuse the tree after clear
    EXPECT_TRUE(tree.insert(30));
    EXPECT_TRUE(tree.insert(40));
    EXPECT_EQ(tree.size(), 2);
    EXPECT_TRUE(tree.contains(30));
    EXPECT_TRUE(tree.contains(40));
}

// Edge cases and stress tests
TEST_F(AVLTreeTest, InsertManyValues) {
    const int NUM_VALUES = 100;
    
    for (int i = 0; i < NUM_VALUES; ++i) {
        EXPECT_TRUE(tree.insert(i));
    }
    
    EXPECT_EQ(tree.size(), NUM_VALUES);
    
    // All values should be present
    for (int i = 0; i < NUM_VALUES; ++i) {
        EXPECT_TRUE(tree.contains(i));
    }
    
    // Tree should be balanced
    int height = tree.getHeight();
    EXPECT_LE(height, 8); // For 100 nodes, AVL height should be <= 8
}

TEST_F(AVLTreeTest, InsertRemoveSequence) {
    // Test insert/remove sequence
    EXPECT_TRUE(tree.insert(10));
    EXPECT_TRUE(tree.insert(20));
    EXPECT_TRUE(tree.insert(5));
    EXPECT_EQ(tree.size(), 3);
    
    EXPECT_TRUE(tree.remove(20));
    EXPECT_EQ(tree.size(), 2);
    EXPECT_FALSE(tree.contains(20));
    
    EXPECT_TRUE(tree.insert(15));
    EXPECT_EQ(tree.size(), 3);
    EXPECT_TRUE(tree.contains(15));
    
    EXPECT_TRUE(tree.remove(5));
    EXPECT_EQ(tree.size(), 2);
    EXPECT_FALSE(tree.contains(5));
    
    EXPECT_TRUE(tree.remove(10));
    EXPECT_EQ(tree.size(), 1);
    EXPECT_TRUE(tree.contains(15));
    
    EXPECT_TRUE(tree.remove(15));
    EXPECT_TRUE(tree.empty());
}

TEST_F(AVLTreeTest, NegativeValues) {
    EXPECT_TRUE(tree.insert(-10));
    EXPECT_TRUE(tree.insert(-5));
    EXPECT_TRUE(tree.insert(-20));
    EXPECT_TRUE(tree.insert(0));
    EXPECT_TRUE(tree.insert(10));
    
    EXPECT_EQ(tree.size(), 5);
    EXPECT_TRUE(tree.contains(-10));
    EXPECT_TRUE(tree.contains(-5));
    EXPECT_TRUE(tree.contains(-20));
    EXPECT_TRUE(tree.contains(0));
    EXPECT_TRUE(tree.contains(10));
}

TEST_F(AVLTreeTest, LargeValues) {
    EXPECT_TRUE(tree.insert(1000000));
    EXPECT_TRUE(tree.insert(-1000000));
    EXPECT_TRUE(tree.insert(0));
    
    EXPECT_EQ(tree.size(), 3);
    EXPECT_TRUE(tree.contains(1000000));
    EXPECT_TRUE(tree.contains(-1000000));
    EXPECT_TRUE(tree.contains(0));
}

TEST_F(AVLTreeTest, SwapOperation) {
    tree.insert(10);
    tree.insert(20);
    
    AVLTree other;
    other.insert(30);
    other.insert(40);
    
    tree.swap(other);
    
    EXPECT_EQ(tree.size(), 2);
    EXPECT_TRUE(tree.contains(30));
    EXPECT_TRUE(tree.contains(40));
    
    EXPECT_EQ(other.size(), 2);
    EXPECT_TRUE(other.contains(10));
    EXPECT_TRUE(other.contains(20));
}