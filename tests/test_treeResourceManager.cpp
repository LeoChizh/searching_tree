#include <gtest/gtest.h>
#include "TreeResourceManager.hpp"

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

TEST_F(TreeResourceManagerTest, CreateNodeBasic) {
    auto [handle, result] = manager->createNode();
    EXPECT_EQ(result, TreeResourceManager::CreateResult::Success);
    EXPECT_TRUE(handle.isValid());
    
    // Test node properties through manager interface
    auto value = manager->getNodeValue(handle);
    EXPECT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), 0);
    
    auto balance = manager->getBalanceFactor(handle);
    EXPECT_TRUE(balance.has_value());
    EXPECT_EQ(balance.value(), 0);
    
    auto height = manager->getHeight(handle);
    EXPECT_TRUE(height.has_value());
    EXPECT_EQ(height.value(), 0);
    
    // Test relationships
    EXPECT_FALSE(manager->getLeftChild(handle).isValid());
    EXPECT_FALSE(manager->getRightChild(handle).isValid());
    EXPECT_FALSE(manager->getParent(handle).isValid());
}

TEST_F(TreeResourceManagerTest, CreateNodeWithValue) {
    auto [handle, result] = manager->createNode(42);
    EXPECT_EQ(result, TreeResourceManager::CreateResult::Success);
    EXPECT_TRUE(handle.isValid());
    
    auto value = manager->getNodeValue(handle);
    EXPECT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), 42);
}

TEST_F(TreeResourceManagerTest, TryCreateNode) {
    TreeResourceHandle handle = manager->tryCreateNode(42);
    EXPECT_TRUE(handle.isValid());
    
    auto value = manager->getNodeValue(handle);
    EXPECT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), 42);
}

TEST_F(TreeResourceManagerTest, CreateMultipleNodes) {
    auto [handle1, result1] = manager->createNode(1);
    auto [handle2, result2] = manager->createNode(2);
    auto [handle3, result3] = manager->createNode(3);
    
    EXPECT_EQ(result1, TreeResourceManager::CreateResult::Success);
    EXPECT_EQ(result2, TreeResourceManager::CreateResult::Success);
    EXPECT_EQ(result3, TreeResourceManager::CreateResult::Success);
    
    EXPECT_TRUE(handle1.isValid());
    EXPECT_TRUE(handle2.isValid());
    EXPECT_TRUE(handle3.isValid());
    
    // Verify they're different handles
    EXPECT_NE(handle1, handle2);
    EXPECT_NE(handle1, handle3);
    EXPECT_NE(handle2, handle3);
    
    // Verify node values through manager
    EXPECT_EQ(manager->getNodeValue(handle1).value(), 1);
    EXPECT_EQ(manager->getNodeValue(handle2).value(), 2);
    EXPECT_EQ(manager->getNodeValue(handle3).value(), 3);
}

TEST_F(TreeResourceManagerTest, DeleteNode) {
    auto [handle, result] = manager->createNode(42);
    EXPECT_EQ(result, TreeResourceManager::CreateResult::Success);
    EXPECT_TRUE(manager->isValidHandle(handle));
    
    bool deleteResult = manager->deleteNode(handle);
    EXPECT_TRUE(deleteResult);
    
    // Verify node is gone
    EXPECT_FALSE(manager->isValidHandle(handle));
    EXPECT_FALSE(manager->getNodeValue(handle).has_value());
}

TEST_F(TreeResourceManagerTest, DeleteInvalidNode) {
    TreeResourceHandle invalidHandle;
    bool result = manager->deleteNode(invalidHandle);
    EXPECT_FALSE(result);
}

TEST_F(TreeResourceManagerTest, IsValidHandle) {
    auto [handle, result] = manager->createNode(42);
    EXPECT_TRUE(manager->isValidHandle(handle));
    
    manager->deleteNode(handle);
    EXPECT_FALSE(manager->isValidHandle(handle));
}

TEST_F(TreeResourceManagerTest, CapacityMethods) {
    EXPECT_EQ(manager->capacity(), 1000000);
    EXPECT_EQ(manager->getTotalNodeCount(), 0);
    EXPECT_EQ(manager->getActiveNodeCount(), 0);
    EXPECT_EQ(manager->getAvailableCapacity(), 1000000);
    
    auto [handle, result] = manager->createNode(42);
    EXPECT_EQ(result, TreeResourceManager::CreateResult::Success);
    
    EXPECT_EQ(manager->getTotalNodeCount(), 1);
    EXPECT_EQ(manager->getActiveNodeCount(), 1);
    EXPECT_EQ(manager->getAvailableCapacity(), 999999);
}

TEST_F(TreeResourceManagerTest, ClearMethod) {
    auto [handle1, result1] = manager->createNode(1);
    auto [handle2, result2] = manager->createNode(2);
    
    EXPECT_EQ(manager->getActiveNodeCount(), 2);
    EXPECT_TRUE(manager->isValidHandle(handle1));
    EXPECT_TRUE(manager->isValidHandle(handle2));
    
    manager->clear();
    
    EXPECT_EQ(manager->getActiveNodeCount(), 0);
    EXPECT_FALSE(manager->isValidHandle(handle1));
    EXPECT_FALSE(manager->isValidHandle(handle2));
    
    // Should be able to create new nodes after clear
    auto [newHandle, newResult] = manager->createNode(3);
    EXPECT_EQ(newResult, TreeResourceManager::CreateResult::Success);
    EXPECT_EQ(manager->getActiveNodeCount(), 1);
}

TEST_F(TreeResourceManagerTest, NodeRelationships) {
    auto [parentHandle, parentResult] = manager->createNode(1);
    auto [leftHandle, leftResult] = manager->createNode(2);
    auto [rightHandle, rightResult] = manager->createNode(3);
    
    // Set up relationships through manager
    EXPECT_TRUE(manager->setLeftChild(parentHandle, leftHandle));
    EXPECT_TRUE(manager->setRightChild(parentHandle, rightHandle));
    
    // Verify relationships
    EXPECT_EQ(manager->getLeftChild(parentHandle), leftHandle);
    EXPECT_EQ(manager->getRightChild(parentHandle), rightHandle);
    EXPECT_EQ(manager->getParent(leftHandle), parentHandle);
    EXPECT_EQ(manager->getParent(rightHandle), parentHandle);
    
    // Verify values
    EXPECT_EQ(manager->getNodeValue(parentHandle).value(), 1);
    EXPECT_EQ(manager->getNodeValue(leftHandle).value(), 2);
    EXPECT_EQ(manager->getNodeValue(rightHandle).value(), 3);
}

TEST_F(TreeResourceManagerTest, SetNodeProperties) {
    auto [handle, result] = manager->createNode(0);
    
    // Test setting properties
    EXPECT_TRUE(manager->setNodeValue(handle, 100));
    EXPECT_TRUE(manager->setBalanceFactor(handle, 1));
    EXPECT_TRUE(manager->setHeight(handle, 5));
    
    // Verify properties
    EXPECT_EQ(manager->getNodeValue(handle).value(), 100);
    EXPECT_EQ(manager->getBalanceFactor(handle).value(), 1);
    EXPECT_EQ(manager->getHeight(handle).value(), 5);
}

TEST_F(TreeResourceManagerTest, SetPropertiesInvalidHandle) {
    TreeResourceHandle invalidHandle;
    
    EXPECT_FALSE(manager->setNodeValue(invalidHandle, 100));
    EXPECT_FALSE(manager->setBalanceFactor(invalidHandle, 1));
    EXPECT_FALSE(manager->setHeight(invalidHandle, 5));
}

TEST_F(TreeResourceManagerTest, SetRelationshipsInvalidHandle) {
    TreeResourceHandle invalidHandle;
    auto [validHandle, result] = manager->createNode(1);
    
    // Should fail: invalid parent
    EXPECT_FALSE(manager->setLeftChild(invalidHandle, validHandle));
    EXPECT_FALSE(manager->setRightChild(invalidHandle, validHandle));
    
    // Should succeed: valid parent with invalid child (creating leaf node)
    EXPECT_TRUE(manager->setLeftChild(validHandle, invalidHandle));
    EXPECT_TRUE(manager->setRightChild(validHandle, invalidHandle));
    
    // Verify the node is now a leaf
    EXPECT_FALSE(manager->getLeftChild(validHandle).isValid());
    EXPECT_FALSE(manager->getRightChild(validHandle).isValid());
    EXPECT_TRUE(manager->getNodeValue(validHandle).has_value()); // Parent still exists
}

TEST_F(TreeResourceManagerTest, ManyNodes) {
    const int NUM_NODES = 1000;
    std::vector<TreeResourceHandle> handles;
    
    for (int i = 0; i < NUM_NODES; ++i) {
        auto [handle, result] = manager->createNode(i);
        EXPECT_EQ(result, TreeResourceManager::CreateResult::Success);
        handles.push_back(handle);
        
        auto value = manager->getNodeValue(handle);
        EXPECT_TRUE(value.has_value());
        EXPECT_EQ(value.value(), i);
    }
    
    EXPECT_EQ(manager->getActiveNodeCount(), NUM_NODES);
}

TEST_F(TreeResourceManagerTest, DeleteRemovesFromManagement) {
    auto [handle1, result1] = manager->createNode(1);
    auto [handle2, result2] = manager->createNode(2);
    
    EXPECT_EQ(manager->getActiveNodeCount(), 2);
    
    // Delete first node
    EXPECT_TRUE(manager->deleteNode(handle1));
    EXPECT_EQ(manager->getActiveNodeCount(), 1);
    
    // Should not be able to access deleted node
    EXPECT_FALSE(manager->isValidHandle(handle1));
    EXPECT_FALSE(manager->getNodeValue(handle1).has_value());
    
    // Second node should still be accessible
    EXPECT_TRUE(manager->isValidHandle(handle2));
    EXPECT_EQ(manager->getNodeValue(handle2).value(), 2);
    
    // Create new nodes after deletion
    auto [handle3, result3] = manager->createNode(3);
    auto [handle4, result4] = manager->createNode(4);
    
    EXPECT_EQ(manager->getActiveNodeCount(), 3);
    EXPECT_EQ(manager->getNodeValue(handle3).value(), 3);
    EXPECT_EQ(manager->getNodeValue(handle4).value(), 4);
}