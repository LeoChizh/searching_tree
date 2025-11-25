#include <gtest/gtest.h>
#include "TreeNode.hpp"

class TreeNodeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test nodes with handles
        root = TreeNode();
        leftChild = TreeNode();
        rightChild = TreeNode();
        
        // Set up relationships using handles
        root.left = TreeResourceHandle{1, 1};  // Mock handles
        root.right = TreeResourceHandle{2, 1};
        leftChild.parent = TreeResourceHandle{0, 1};  // root handle
        rightChild.parent = TreeResourceHandle{0, 1}; // root handle
        
        root.value = 10;
        leftChild.value = 5;
        rightChild.value = 15;
    }
    
    TreeNode root;
    TreeNode leftChild;
    TreeNode rightChild;
};

// Test default constructor
TEST_F(TreeNodeTest, DefaultConstructor) {
    TreeNode node;
    
    EXPECT_FALSE(node.parent.isValid());
    EXPECT_FALSE(node.left.isValid());
    EXPECT_FALSE(node.right.isValid());
    EXPECT_EQ(node.value, 0);
    EXPECT_EQ(node.balanceFactor, 0);
    EXPECT_EQ(node.height, 0);
}

// Test isLeaf method
TEST_F(TreeNodeTest, IsLeaf) {
    TreeNode leafNode;
    EXPECT_TRUE(leafNode.isLeaf());
    
    TreeNode nonLeafNode;
    nonLeafNode.left = TreeResourceHandle{1, 1};
    EXPECT_FALSE(nonLeafNode.isLeaf());
}

TEST_F(TreeNodeTest, IsLeafWithChildren) {
    EXPECT_FALSE(root.isLeaf());  // Has both children
    EXPECT_TRUE(leftChild.isLeaf());  // No children (handles are invalid)
    EXPECT_TRUE(rightChild.isLeaf());  // No children (handles are invalid)
}

// Test isRoot method
TEST_F(TreeNodeTest, IsRoot) {
    TreeNode rootNode;
    EXPECT_TRUE(rootNode.isRoot());  // No parent
    
    TreeNode childNode;
    childNode.parent = TreeResourceHandle{1, 1};
    EXPECT_FALSE(childNode.isRoot());  // Has parent
}

// Test hasLeft method
TEST_F(TreeNodeTest, HasLeft) {
    TreeNode nodeWithLeft;
    nodeWithLeft.left = TreeResourceHandle{1, 1};
    EXPECT_TRUE(nodeWithLeft.hasLeft());
    
    TreeNode nodeWithoutLeft;
    EXPECT_FALSE(nodeWithoutLeft.hasLeft());
}

// Test hasRight method
TEST_F(TreeNodeTest, HasRight) {
    TreeNode nodeWithRight;
    nodeWithRight.right = TreeResourceHandle{1, 1};
    EXPECT_TRUE(nodeWithRight.hasRight());
    
    TreeNode nodeWithoutRight;
    EXPECT_FALSE(nodeWithoutRight.hasRight());
}

// Test value assignment and retrieval
TEST_F(TreeNodeTest, ValueAssignment) {
    TreeNode node;
    node.value = 100;
    EXPECT_EQ(node.value, 100);
    
    node.value = -50;
    EXPECT_EQ(node.value, -50);
}

// Test balanceFactor assignment and retrieval
TEST_F(TreeNodeTest, BalanceFactorAssignment) {
    TreeNode node;
    node.balanceFactor = 1;
    EXPECT_EQ(node.balanceFactor, 1);
    
    node.balanceFactor = -1;
    EXPECT_EQ(node.balanceFactor, -1);
    
    node.balanceFactor = 0;
    EXPECT_EQ(node.balanceFactor, 0);
}

// Test height assignment and retrieval
TEST_F(TreeNodeTest, HeightAssignment) {
    TreeNode node;
    node.height = 5;
    EXPECT_EQ(node.height, 5);
    
    node.height = 0;
    EXPECT_EQ(node.height, 0);
    
    node.height = 100;
    EXPECT_EQ(node.height, 100);
}

// Test handle validity
TEST_F(TreeNodeTest, HandleValidity) {
    TreeNode node;
    
    // Test invalid handles
    EXPECT_FALSE(node.parent.isValid());
    EXPECT_FALSE(node.left.isValid());
    EXPECT_FALSE(node.right.isValid());
    
    // Test valid handles
    node.parent = TreeResourceHandle{0, 1};
    node.left = TreeResourceHandle{1, 1};
    node.right = TreeResourceHandle{2, 1};
    
    EXPECT_TRUE(node.parent.isValid());
    EXPECT_TRUE(node.left.isValid());
    EXPECT_TRUE(node.right.isValid());
}

// Test edge cases for query methods
TEST_F(TreeNodeTest, EdgeCases) {
    TreeNode node;
    
    // Test all query methods on a fresh node
    EXPECT_TRUE(node.isLeaf());
    EXPECT_TRUE(node.isRoot());
    EXPECT_FALSE(node.hasLeft());
    EXPECT_FALSE(node.hasRight());
    
    // Add only left child
    node.left = TreeResourceHandle{1, 1};
    EXPECT_FALSE(node.isLeaf());
    EXPECT_TRUE(node.isRoot());
    EXPECT_TRUE(node.hasLeft());
    EXPECT_FALSE(node.hasRight());
    
    // Add only right child
    TreeNode node2;
    node2.right = TreeResourceHandle{1, 1};
    EXPECT_FALSE(node2.isLeaf());
    EXPECT_TRUE(node2.isRoot());
    EXPECT_FALSE(node2.hasLeft());
    EXPECT_TRUE(node2.hasRight());
}

// Test node with only one child
TEST_F(TreeNodeTest, SingleChildNode) {
    TreeNode parent;
    TreeNode child;
    
    parent.left = TreeResourceHandle{1, 1};  // Mock child handle
    child.parent = TreeResourceHandle{0, 1}; // Mock parent handle
    
    EXPECT_FALSE(parent.isLeaf());
    EXPECT_TRUE(parent.isRoot());
    EXPECT_TRUE(parent.hasLeft());
    EXPECT_FALSE(parent.hasRight());
    
    EXPECT_TRUE(child.isLeaf());
    EXPECT_FALSE(child.isRoot());
    EXPECT_FALSE(child.hasLeft());
    EXPECT_FALSE(child.hasRight());
}