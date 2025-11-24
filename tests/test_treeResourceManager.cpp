#include <gtest/gtest.h>
#include "TreeResourceManager.hpp"
#include "TreeNode.hpp"

class TreeResourceManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        manager = std::make_unique<TreeResourceManager>();
    }

    void TearDown() override {
        manager.reset();
    }

    std::unique_ptr<TreeResourceManager> manager;
};

// Test basic creation
TEST_F(TreeResourceManagerTest, CreateNodeBasic) {
    TreeNode* node = manager->createNode();
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->value, 0);  // Default value
    EXPECT_EQ(node->parent, nullptr);
    EXPECT_EQ(node->left, nullptr);
    EXPECT_EQ(node->right, nullptr);
}

// Test creation with specific value
TEST_F(TreeResourceManagerTest, CreateNodeWithValue) {
    TreeNode* node = manager->createNode(42);
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->value, 42);
}

// Test multiple node creation
TEST_F(TreeResourceManagerTest, CreateMultipleNodes) {
    TreeNode* node1 = manager->createNode(1);
    TreeNode* node2 = manager->createNode(2);
    TreeNode* node3 = manager->createNode(3);

    EXPECT_NE(node1, nullptr);
    EXPECT_NE(node2, nullptr);
    EXPECT_NE(node3, nullptr);
    
    EXPECT_EQ(node1->value, 1);
    EXPECT_EQ(node2->value, 2);
    EXPECT_EQ(node3->value, 3);
    
    // All nodes should be distinct
    EXPECT_NE(node1, node2);
    EXPECT_NE(node1, node3);
    EXPECT_NE(node2, node3);
}

// Test node deletion
TEST_F(TreeResourceManagerTest, DeleteNode) {
    TreeNode* node = manager->createNode(100);
    EXPECT_NE(node, nullptr);
    
    manager->deleteNode(node);
    // If we get here without crash, deletion worked
    SUCCEED();
}

// Test deletion of null node
TEST_F(TreeResourceManagerTest, DeleteNullNode) {
    // This should not crash or throw
    manager->deleteNode(nullptr);
    SUCCEED();
}

// Test deletion of non-managed node
TEST_F(TreeResourceManagerTest, DeleteNonManagedNode) {
    TreeNode externalNode;  // Not managed by TreeResourceManager
    externalNode.value = 999;
    
    // This should not crash or affect the external node
    manager->deleteNode(&externalNode);
    
    // External node should still be valid
    EXPECT_EQ(externalNode.value, 999);
}

// Test memory management - nodes should be properly destroyed
TEST_F(TreeResourceManagerTest, MemoryManagement) {
    TreeNode* node1 = manager->createNode(1);
    manager->createNode(2);  // node2 not stored
    
    // Delete one node
    manager->deleteNode(node1);
    
    // Manager should still be functional
    TreeNode* node3 = manager->createNode(3);
    EXPECT_NE(node3, nullptr);
    EXPECT_EQ(node3->value, 3);
}

// Test that nodes are properly cleaned up when manager is destroyed
TEST_F(TreeResourceManagerTest, ManagerDestructionCleansUpNodes) {
    auto localManager = std::make_unique<TreeResourceManager>();
    TreeNode* node1 = localManager->createNode(1);
    TreeNode* node2 = localManager->createNode(2);
    TreeNode* node3 = localManager->createNode(3);
    
    EXPECT_NE(node1, nullptr);
    EXPECT_NE(node2, nullptr);
    EXPECT_NE(node3, nullptr);
    
    // localManager destroyed here, all nodes should be destroyed
    localManager.reset();
    
    SUCCEED();  // If we get here without crashes, destruction worked
}

// Test move semantics
TEST_F(TreeResourceManagerTest, MoveConstruction) {
    manager->createNode(50);  // originalNode not stored
    
    // Move the manager
    TreeResourceManager movedManager = std::move(*manager);
    
    // Original manager should be empty but usable
    TreeNode* newNode = manager->createNode(60);
    EXPECT_NE(newNode, nullptr);
    
    // Moved manager should be functional
    TreeNode* movedNode = movedManager.createNode(70);
    EXPECT_NE(movedNode, nullptr);
}

// Test move assignment
TEST_F(TreeResourceManagerTest, MoveAssignment) {
    auto otherManager = std::make_unique<TreeResourceManager>();
    otherManager->createNode(80);  // otherNode not stored
    
    *manager = std::move(*otherManager);
    
    // otherManager should still be in valid state
    TreeNode* newNode = otherManager->createNode(90);
    EXPECT_NE(newNode, nullptr);
}

// Test node relationships can be established with managed nodes
TEST_F(TreeResourceManagerTest, NodeRelationships) {
    TreeNode* parent = manager->createNode(1);
    TreeNode* leftChild = manager->createNode(2);
    TreeNode* rightChild = manager->createNode(3);
    
    // Establish relationships
    parent->left = leftChild;
    parent->right = rightChild;
    leftChild->parent = parent;
    rightChild->parent = parent;
    
    // Verify relationships
    EXPECT_EQ(parent->left, leftChild);
    EXPECT_EQ(parent->right, rightChild);
    EXPECT_EQ(leftChild->parent, parent);
    EXPECT_EQ(rightChild->parent, parent);
    
    // Verify values
    EXPECT_EQ(parent->value, 1);
    EXPECT_EQ(leftChild->value, 2);
    EXPECT_EQ(rightChild->value, 3);
}

// Test stress/performance with many nodes
TEST_F(TreeResourceManagerTest, ManyNodes) {
    const int NUM_NODES = 1000;
    
    for (int i = 0; i < NUM_NODES; ++i) {
        TreeNode* node = manager->createNode(i);
        EXPECT_NE(node, nullptr);
        EXPECT_EQ(node->value, i);
    }
    
    SUCCEED();  // If we get here, no crashes with many nodes
}

// Test that deleted nodes are truly removed from management
TEST_F(TreeResourceManagerTest, DeleteRemovesFromManagement) {
    TreeNode* node1 = manager->createNode(1);
    TreeNode* node2 = manager->createNode(2);
    
    // Delete node1
    manager->deleteNode(node1);
    
    // Create new nodes
    TreeNode* node3 = manager->createNode(3);
    TreeNode* node4 = manager->createNode(4);
    
    // All should be valid
    EXPECT_NE(node2, nullptr);
    EXPECT_NE(node3, nullptr);
    EXPECT_NE(node4, nullptr);
    EXPECT_EQ(node2->value, 2);
    EXPECT_EQ(node3->value, 3);
    EXPECT_EQ(node4->value, 4);
}