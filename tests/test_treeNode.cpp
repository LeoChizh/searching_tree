#include <gtest/gtest.h>
#include "TreeNode.hpp"

class TreeNodeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create some test nodes
        root = new TreeNode();
        leftChild = new TreeNode();
        rightChild = new TreeNode();
        
        // Set up relationships
        root->left = leftChild;
        root->right = rightChild;
        leftChild->parent = root;
        rightChild->parent = root;
        
        root->value = 10;
        leftChild->value = 5;
        rightChild->value = 15;
    }
    
    void TearDown() override {
        delete leftChild;
        delete rightChild;
        delete root;
    }
    
    TreeNode* root = nullptr;
    TreeNode* leftChild = nullptr;
    TreeNode* rightChild = nullptr;
};

// Test default constructor
TEST_F(TreeNodeTest, DefaultConstructor) {
    TreeNode node;
    
    EXPECT_EQ(node.parent, nullptr);
    EXPECT_EQ(node.left, nullptr);
    EXPECT_EQ(node.right, nullptr);
    EXPECT_EQ(node.value, 0);
    EXPECT_EQ(node.balancer, 0);
}

// Test parameterized constructor
TEST_F(TreeNodeTest, ParameterizedConstructor) {
    TreeNode* parent = new TreeNode();
    TreeNode* left = new TreeNode();
    TreeNode* right = new TreeNode();
    
    TreeNode node(parent, left, right, 42);
    
    EXPECT_EQ(node.parent, parent);
    EXPECT_EQ(node.left, left);
    EXPECT_EQ(node.right, right);
    EXPECT_EQ(node.value, 42);
    
    delete left;
    delete right;
    delete parent;
}

// Test isLeaf method
TEST_F(TreeNodeTest, IsLeaf) {
    TreeNode leafNode;
    EXPECT_TRUE(leafNode.isLeaf());
    
    TreeNode nonLeafNode;
    nonLeafNode.left = new TreeNode();
    EXPECT_FALSE(nonLeafNode.isLeaf());
    
    delete nonLeafNode.left;
}

TEST_F(TreeNodeTest, IsLeafWithChildren) {
    EXPECT_FALSE(root->isLeaf());  // Has both children
    EXPECT_TRUE(leftChild->isLeaf());  // No children
    EXPECT_TRUE(rightChild->isLeaf());  // No children
}

// Test isRoot method
TEST_F(TreeNodeTest, IsRoot) {
    EXPECT_TRUE(root->isRoot());  // No parent
    EXPECT_FALSE(leftChild->isRoot());  // Has parent
    EXPECT_FALSE(rightChild->isRoot());  // Has parent
}

// Test hasLeft method
TEST_F(TreeNodeTest, HasLeft) {
    EXPECT_TRUE(root->hasLeft());  // Has left child
    EXPECT_FALSE(leftChild->hasLeft());  // No left child
    EXPECT_FALSE(rightChild->hasLeft());  // No left child
}

// Test hasRight method
TEST_F(TreeNodeTest, HasRight) {
    EXPECT_TRUE(root->hasRight());  // Has right child
    EXPECT_FALSE(leftChild->hasRight());  // No right child
    EXPECT_FALSE(rightChild->hasRight());  // No right child
}

// Test value assignment and retrieval
TEST_F(TreeNodeTest, ValueAssignment) {
    TreeNode node;
    node.value = 100;
    EXPECT_EQ(node.value, 100);
    
    node.value = -50;
    EXPECT_EQ(node.value, -50);
}

// Test balancer assignment and retrieval
TEST_F(TreeNodeTest, BalancerAssignment) {
    TreeNode node;
    node.balancer = 1;
    EXPECT_EQ(node.balancer, 1);
    
    node.balancer = -1;
    EXPECT_EQ(node.balancer, -1);
    
    node.balancer = 0;
    EXPECT_EQ(node.balancer, 0);
}

// Test parent-child relationships
TEST_F(TreeNodeTest, ParentChildRelationships) {
    EXPECT_EQ(leftChild->parent, root);
    EXPECT_EQ(rightChild->parent, root);
    EXPECT_EQ(root->left, leftChild);
    EXPECT_EQ(root->right, rightChild);
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
    node.left = new TreeNode();
    EXPECT_FALSE(node.isLeaf());
    EXPECT_TRUE(node.isRoot());
    EXPECT_TRUE(node.hasLeft());
    EXPECT_FALSE(node.hasRight());
    
    // Add only right child
    TreeNode node2;
    node2.right = new TreeNode();
    EXPECT_FALSE(node2.isLeaf());
    EXPECT_TRUE(node2.isRoot());
    EXPECT_FALSE(node2.hasLeft());
    EXPECT_TRUE(node2.hasRight());
    
    delete node.left;
    delete node2.right;
}

// Test node with only one child
TEST_F(TreeNodeTest, SingleChildNode) {
    TreeNode parent;
    TreeNode child;
    
    parent.left = &child;
    child.parent = &parent;
    
    EXPECT_FALSE(parent.isLeaf());
    EXPECT_TRUE(parent.isRoot());
    EXPECT_TRUE(parent.hasLeft());
    EXPECT_FALSE(parent.hasRight());
    
    EXPECT_TRUE(child.isLeaf());
    EXPECT_FALSE(child.isRoot());
    EXPECT_FALSE(child.hasLeft());
    EXPECT_FALSE(child.hasRight());
}

// Test complex tree structure
TEST_F(TreeNodeTest, ComplexTreeStructure) {
    TreeNode* grandparent = new TreeNode();
    TreeNode* parent = new TreeNode();
    TreeNode* child1 = new TreeNode();
    TreeNode* child2 = new TreeNode();
    
    // Build: grandparent -> parent -> (child1, child2)
    grandparent->left = parent;
    parent->parent = grandparent;
    parent->left = child1;
    parent->right = child2;
    child1->parent = parent;
    child2->parent = parent;
    
    EXPECT_TRUE(grandparent->isRoot());
    EXPECT_FALSE(grandparent->isLeaf());
    EXPECT_TRUE(grandparent->hasLeft());
    EXPECT_FALSE(grandparent->hasRight());
    
    EXPECT_FALSE(parent->isRoot());
    EXPECT_FALSE(parent->isLeaf());
    EXPECT_TRUE(parent->hasLeft());
    EXPECT_TRUE(parent->hasRight());
    
    EXPECT_TRUE(child1->isLeaf());
    EXPECT_FALSE(child1->isRoot());
    
    delete grandparent;
    delete parent;
    delete child1;
    delete child2;
}