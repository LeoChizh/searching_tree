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

// Test basic creation with new API
TEST_F(TreeResourceManagerTest, CreateNodeBasic) {
    auto [node, result] = manager->createNode();
    EXPECT_EQ(result, TreeResourceManager::CreateResult::Success);
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->value, 0);
    EXPECT_EQ(node->parent, nullptr);
    EXPECT_EQ(node->left, nullptr);
    EXPECT_EQ(node->right, nullptr);
}

// Test creation with specific value
TEST_F(TreeResourceManagerTest, CreateNodeWithValue) {
    auto [node, result] = manager->createNode(42);
    EXPECT_EQ(result, TreeResourceManager::CreateResult::Success);
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->value, 42);
}

// Test tryCreateNode method
TEST_F(TreeResourceManagerTest, TryCreateNode) {
    TreeNode* node = manager->tryCreateNode(42);
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->value, 42);
    
    TreeNode* nullNode = manager->tryCreateNode(); // Default value
    EXPECT_NE(nullNode, nullptr);
    EXPECT_EQ(nullNode->value, 0);
}

// Test multiple node creation
TEST_F(TreeResourceManagerTest, CreateMultipleNodes) {
    auto [node1, result1] = manager->createNode(1);
    auto [node2, result2] = manager->createNode(2);
    auto [node3, result3] = manager->createNode(3);

    EXPECT_EQ(result1, TreeResourceManager::CreateResult::Success);
    EXPECT_EQ(result2, TreeResourceManager::CreateResult::Success);
    EXPECT_EQ(result3, TreeResourceManager::CreateResult::Success);
    
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

// Test node deletion with return value
TEST_F(TreeResourceManagerTest, DeleteNode) {
    auto [node, result] = manager->createNode(100);
    EXPECT_EQ(result, TreeResourceManager::CreateResult::Success);
    EXPECT_NE(node, nullptr);
    
    bool deleteResult = manager->deleteNode(node);
    EXPECT_TRUE(deleteResult);
}

// Test deletion of null node
TEST_F(TreeResourceManagerTest, DeleteNullNode) {
    bool result = manager->deleteNode(nullptr);
    EXPECT_FALSE(result);
}

// Test deletion of non-managed node
TEST_F(TreeResourceManagerTest, DeleteNonManagedNode) {
    TreeNode externalNode;
    externalNode.value = 999;
    
    bool result = manager->deleteNode(&externalNode);
    EXPECT_FALSE(result);
    EXPECT_EQ(externalNode.value, 999);
}

// Test contains method
TEST_F(TreeResourceManagerTest, ContainsMethod) {
    auto [node, result] = manager->createNode(42);
    EXPECT_EQ(result, TreeResourceManager::CreateResult::Success);
    
    EXPECT_TRUE(manager->contains(node));
    
    manager->deleteNode(node);
    EXPECT_FALSE(manager->contains(node));
}

// Test capacity methods
TEST_F(TreeResourceManagerTest, CapacityMethods) {
    EXPECT_GT(manager->capacity(), 0);
    EXPECT_EQ(manager->getManagedNodeCount(), 0);
    EXPECT_EQ(manager->getAvailableCapacity(), manager->capacity());
    
    auto [node, result] = manager->createNode(42);
    EXPECT_EQ(result, TreeResourceManager::CreateResult::Success);
    
    EXPECT_EQ(manager->getManagedNodeCount(), 1);
    EXPECT_EQ(manager->getAvailableCapacity(), manager->capacity() - 1);
}

// Test clear method
TEST_F(TreeResourceManagerTest, ClearMethod) {
    auto [node1, result1] = manager->createNode(1);
    auto [node2, result2] = manager->createNode(2);
    
    EXPECT_EQ(manager->getManagedNodeCount(), 2);
    EXPECT_TRUE(manager->contains(node1));
    EXPECT_TRUE(manager->contains(node2));
    
    manager->clear();
    
    EXPECT_EQ(manager->getManagedNodeCount(), 0);
    EXPECT_FALSE(manager->contains(node1));
    EXPECT_FALSE(manager->contains(node2));
    
    // Should be able to create new nodes after clear
    auto [node3, result3] = manager->createNode(3);
    EXPECT_EQ(result3, TreeResourceManager::CreateResult::Success);
    EXPECT_EQ(manager->getManagedNodeCount(), 1);
}

// Test memory management
TEST_F(TreeResourceManagerTest, MemoryManagement) {
    auto [node1, result1] = manager->createNode(1);
    manager->createNode(2);  // node2 not stored
    
    EXPECT_EQ(manager->getManagedNodeCount(), 2);
    
    bool deleteResult = manager->deleteNode(node1);
    EXPECT_TRUE(deleteResult);
    EXPECT_EQ(manager->getManagedNodeCount(), 1);
    
    // Manager should still be functional
    auto [node3, result3] = manager->createNode(3);
    EXPECT_EQ(result3, TreeResourceManager::CreateResult::Success);
    EXPECT_EQ(manager->getManagedNodeCount(), 2);
}

// Test that nodes are properly cleaned up when manager is destroyed
TEST_F(TreeResourceManagerTest, ManagerDestructionCleansUpNodes) {
    auto localManager = std::make_unique<TreeResourceManager>();
    auto [node1, result1] = localManager->createNode(1);
    auto [node2, result2] = localManager->createNode(2);
    auto [node3, result3] = localManager->createNode(3);
    
    EXPECT_EQ(result1, TreeResourceManager::CreateResult::Success);
    EXPECT_EQ(result2, TreeResourceManager::CreateResult::Success);
    EXPECT_EQ(result3, TreeResourceManager::CreateResult::Success);
    EXPECT_EQ(localManager->getManagedNodeCount(), 3);
    
    // localManager destroyed here, all nodes should be destroyed
    localManager.reset();
    
    SUCCEED();  // If we get here without crashes, destruction worked
}

// Test move semantics
TEST_F(TreeResourceManagerTest, MoveConstruction) {
    auto [node, result] = manager->createNode(50);
    EXPECT_EQ(result, TreeResourceManager::CreateResult::Success);
    
    // Move the manager
    TreeResourceManager movedManager = std::move(*manager);
    
    // Original manager should be empty but usable
    auto [newNode, newResult] = manager->createNode(60);
    EXPECT_EQ(newResult, TreeResourceManager::CreateResult::Success);
    EXPECT_NE(newNode, nullptr);
    
    // Moved manager should be functional
    auto [movedNode, movedResult] = movedManager.createNode(70);
    EXPECT_EQ(movedResult, TreeResourceManager::CreateResult::Success);
    EXPECT_NE(movedNode, nullptr);
}

// Test move assignment
TEST_F(TreeResourceManagerTest, MoveAssignment) {
    auto otherManager = std::make_unique<TreeResourceManager>();
    auto [otherNode, otherResult] = otherManager->createNode(80);
    EXPECT_EQ(otherResult, TreeResourceManager::CreateResult::Success);
    
    *manager = std::move(*otherManager);
    
    // otherManager should still be in valid state
    auto [newNode, newResult] = otherManager->createNode(90);
    EXPECT_EQ(newResult, TreeResourceManager::CreateResult::Success);
    EXPECT_NE(newNode, nullptr);
}

// Test node relationships can be established with managed nodes
TEST_F(TreeResourceManagerTest, NodeRelationships) {
    auto [parent, parentResult] = manager->createNode(1);
    auto [leftChild, leftResult] = manager->createNode(2);
    auto [rightChild, rightResult] = manager->createNode(3);
    
    EXPECT_EQ(parentResult, TreeResourceManager::CreateResult::Success);
    EXPECT_EQ(leftResult, TreeResourceManager::CreateResult::Success);
    EXPECT_EQ(rightResult, TreeResourceManager::CreateResult::Success);
    
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
        auto [node, result] = manager->createNode(i);
        EXPECT_EQ(result, TreeResourceManager::CreateResult::Success);
        EXPECT_NE(node, nullptr);
        EXPECT_EQ(node->value, i);
    }
    
    EXPECT_EQ(manager->getManagedNodeCount(), NUM_NODES);
    SUCCEED();
}

// Test that deleted nodes are truly removed from management
TEST_F(TreeResourceManagerTest, DeleteRemovesFromManagement) {
    auto [node1, result1] = manager->createNode(1);
    auto [node2, result2] = manager->createNode(2);
    
    EXPECT_EQ(result1, TreeResourceManager::CreateResult::Success);
    EXPECT_EQ(result2, TreeResourceManager::CreateResult::Success);
    EXPECT_EQ(manager->getManagedNodeCount(), 2);
    
    // Delete node1
    bool deleteResult = manager->deleteNode(node1);
    EXPECT_TRUE(deleteResult);
    EXPECT_EQ(manager->getManagedNodeCount(), 1);
    
    // Create new nodes
    auto [node3, result3] = manager->createNode(3);
    auto [node4, result4] = manager->createNode(4);
    
    EXPECT_EQ(result3, TreeResourceManager::CreateResult::Success);
    EXPECT_EQ(result4, TreeResourceManager::CreateResult::Success);
    EXPECT_EQ(manager->getManagedNodeCount(), 3);
    
    // All should be valid
    EXPECT_NE(node2, nullptr);
    EXPECT_NE(node3, nullptr);
    EXPECT_NE(node4, nullptr);
    EXPECT_EQ(node2->value, 2);
    EXPECT_EQ(node3->value, 3);
    EXPECT_EQ(node4->value, 4);
}

// Test custom constructor with capacity
TEST_F(TreeResourceManagerTest, CustomCapacity) {
    TreeResourceManager smallManager(5);  // Small capacity for testing
    
    // Fill up to capacity
    for (int i = 0; i < 5; ++i) {
        auto [node, result] = smallManager.createNode(i);
        EXPECT_EQ(result, TreeResourceManager::CreateResult::Success);
    }
    
    // Next creation should fail
    auto [node, result] = smallManager.createNode(6);
    EXPECT_EQ(result, TreeResourceManager::CreateResult::TooManyNodes);
    EXPECT_EQ(node, nullptr);
}

// Test deleteNode return value accuracy
TEST_F(TreeResourceManagerTest, DeleteNodeReturnValueAccuracy) {
    auto [node, result] = manager->createNode(42);
    EXPECT_EQ(result, TreeResourceManager::CreateResult::Success);
    
    // First deletion should succeed
    bool firstDelete = manager->deleteNode(node);
    EXPECT_TRUE(firstDelete);
    
    // Second deletion should fail
    bool secondDelete = manager->deleteNode(node);
    EXPECT_FALSE(secondDelete);
    
    // Deletion of unknown node should fail
    TreeNode externalNode;
    bool externalDelete = manager->deleteNode(&externalNode);
    EXPECT_FALSE(externalDelete);
}