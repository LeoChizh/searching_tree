#include <gtest/gtest.h>
#include <TreeResourceManager.hpp>
#include <TreeResourceHandle.hpp>

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
    
    TreeNode* node = manager->getNode(handle);
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->value, 0);
    EXPECT_EQ(node->parent, nullptr);
    EXPECT_EQ(node->left, nullptr);
    EXPECT_EQ(node->right, nullptr);
}

TEST_F(TreeResourceManagerTest, CreateNodeWithValue) {
    auto [handle, result] = manager->createNode(42);
    EXPECT_EQ(result, TreeResourceManager::CreateResult::Success);
    EXPECT_TRUE(handle.isValid());
    
    TreeNode* node = manager->getNode(handle);
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->value, 42);
}

TEST_F(TreeResourceManagerTest, TryCreateNode) {
    TreeResourceManager::NodeHandle handle = manager->tryCreateNode(42);
    EXPECT_TRUE(handle.isValid());
    
    TreeNode* node = manager->getNode(handle);
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->value, 42);
    
    TreeResourceManager::NodeHandle defaultHandle = manager->tryCreateNode(); // Default value
    EXPECT_TRUE(defaultHandle.isValid());
    
    TreeNode* defaultNode = manager->getNode(defaultHandle);
    EXPECT_NE(defaultNode, nullptr);
    EXPECT_EQ(defaultNode->value, 0);
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
    
    // Verify node values
    TreeNode* node1 = manager->getNode(handle1);
    TreeNode* node2 = manager->getNode(handle2);
    TreeNode* node3 = manager->getNode(handle3);
    
    EXPECT_EQ(node1->value, 1);
    EXPECT_EQ(node2->value, 2);
    EXPECT_EQ(node3->value, 3);
}

TEST_F(TreeResourceManagerTest, DeleteNode) {
    auto [handle, result] = manager->createNode(42);
    EXPECT_EQ(result, TreeResourceManager::CreateResult::Success);
    
    TreeNode* node = manager->getNode(handle);
    EXPECT_NE(node, nullptr);
    
    bool deleteResult = manager->deleteNode(handle);
    EXPECT_TRUE(deleteResult);
    
    // Verify node is gone
    TreeNode* deletedNode = manager->getNode(handle);
    EXPECT_EQ(deletedNode, nullptr);
}

TEST_F(TreeResourceManagerTest, DeleteInvalidNode) {
    TreeResourceManager::NodeHandle invalidHandle;
    bool result = manager->deleteNode(invalidHandle);
    EXPECT_FALSE(result);
}

TEST_F(TreeResourceManagerTest, DeleteNonManagedNode) {
    TreeNode externalNode;
    // Create a fake handle that doesn't exist in manager
    TreeResourceManager::NodeHandle fakeHandle{999, 999};
    
    bool result = manager->deleteNode(fakeHandle);
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

TEST_F(TreeResourceManagerTest, MemoryManagement) {
    auto [handle1, result1] = manager->createNode(1);
    auto [handle2, result2] = manager->createNode(2);
    
    std::cout << "DEBUG: handle1: index=" << handle1.index << ", gen=" << handle1.generation << std::endl;
    std::cout << "DEBUG: handle2: index=" << handle2.index << ", gen=" << handle2.generation << std::endl;
    std::cout << "DEBUG: Active count after creation: " << manager->getActiveNodeCount() << std::endl;
    
    // Delete first node
    bool deleteResult = manager->deleteNode(handle1);
    std::cout << "DEBUG: Delete result: " << deleteResult << std::endl;
    std::cout << "DEBUG: Active count after delete: " << manager->getActiveNodeCount() << std::endl;
    
    // Create new node - should reuse the slot
    auto [handle3, result3] = manager->createNode(3);
    std::cout << "DEBUG: handle3: index=" << handle3.index << ", gen=" << handle3.generation << std::endl;
    std::cout << "DEBUG: Create result: " << (int)result3 << std::endl;
    std::cout << "DEBUG: Active count after recreate: " << manager->getActiveNodeCount() << std::endl;
    std::cout << "DEBUG: handle1 == handle3: " << (handle1 == handle3) << std::endl;
    
    EXPECT_EQ(manager->getActiveNodeCount(), 2);
    EXPECT_EQ(result3, TreeResourceManager::CreateResult::Success);
    EXPECT_EQ(manager->getActiveNodeCount(), 2);
    EXPECT_NE(handle1, handle3);
}

TEST_F(TreeResourceManagerTest, ManagerDestructionCleansUpNodes) {
    // Use a scope to ensure manager destruction
    {
        auto localManager = std::make_unique<TreeResourceManager>();
        
        // Create nodes and verify they exist
        auto [handle1, result1] = localManager->createNode(1);
        auto [handle2, result2] = localManager->createNode(2);
        auto [handle3, result3] = localManager->createNode(3);
        
        EXPECT_EQ(result1, TreeResourceManager::CreateResult::Success);
        EXPECT_EQ(result2, TreeResourceManager::CreateResult::Success);
        EXPECT_EQ(result3, TreeResourceManager::CreateResult::Success);
        EXPECT_EQ(localManager->getActiveNodeCount(), 3);
        
        // Verify nodes are accessible
        EXPECT_NE(localManager->getNode(handle1), nullptr);
        EXPECT_NE(localManager->getNode(handle2), nullptr);
        EXPECT_NE(localManager->getNode(handle3), nullptr);
        
        // Manager goes out of scope here and should destroy all nodes
    }
    
    // If we get here without memory leaks/crashes, the test passes
    // The real verification is done by sanitizers/valgrind
    SUCCEED() << "Manager destruction completed without issues";
}

TEST_F(TreeResourceManagerTest, NodeRelationships) {
    auto [parentHandle, parentResult] = manager->createNode(1);
    auto [leftHandle, leftResult] = manager->createNode(2);
    auto [rightHandle, rightResult] = manager->createNode(3);
    
    TreeNode* parent = manager->getNode(parentHandle);
    TreeNode* leftChild = manager->getNode(leftHandle);
    TreeNode* rightChild = manager->getNode(rightHandle);
    
    // Set up relationships
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

TEST_F(TreeResourceManagerTest, ManyNodes) {
    const int NUM_NODES = 1000;
    std::vector<TreeResourceManager::NodeHandle> handles;
    
    for (int i = 0; i < NUM_NODES; ++i) {
        auto [handle, result] = manager->createNode(i);
        EXPECT_EQ(result, TreeResourceManager::CreateResult::Success);
        handles.push_back(handle);
        
        TreeNode* node = manager->getNode(handle);
        EXPECT_NE(node, nullptr);
        EXPECT_EQ(node->value, i);
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
    TreeNode* deletedNode = manager->getNode(handle1);
    EXPECT_EQ(deletedNode, nullptr);
    
    // Second node should still be accessible
    TreeNode* node2 = manager->getNode(handle2);
    EXPECT_NE(node2, nullptr);
    EXPECT_EQ(node2->value, 2);
    
    // Create new nodes after deletion
    auto [handle3, result3] = manager->createNode(3);
    auto [handle4, result4] = manager->createNode(4);
    
    EXPECT_EQ(manager->getActiveNodeCount(), 3);
    
    TreeNode* node3 = manager->getNode(handle3);
    TreeNode* node4 = manager->getNode(handle4);
    EXPECT_EQ(node3->value, 3);
    EXPECT_EQ(node4->value, 4);
}

TEST_F(TreeResourceManagerTest, DeleteNodeReturnValueAccuracy) {
    TreeResourceManager::NodeHandle invalidHandle;
    bool invalidDelete = manager->deleteNode(invalidHandle);
    EXPECT_FALSE(invalidDelete);
    
    auto [handle, result] = manager->createNode(42);
    bool validDelete = manager->deleteNode(handle);
    EXPECT_TRUE(validDelete);
    
    bool doubleDelete = manager->deleteNode(handle);
    EXPECT_FALSE(doubleDelete);
}