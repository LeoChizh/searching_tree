#include <gtest/gtest.h>
#include "TreeNodePool.hpp"
#include "TreeResourceHandle.hpp"

using NodeHandle = TreeResourceHandle;

class TreeNodePoolTest : public ::testing::Test {
protected:
    TreeNodePool pool;
};

TEST_F(TreeNodePoolTest, CreateNodeBasic) {
    auto [handle, result] = pool.createNode();
    EXPECT_EQ(result, TreeNodePool::CreateResult::Success);
    EXPECT_TRUE(handle.isValid());
    EXPECT_TRUE(pool.isValidHandle(handle));
}

TEST_F(TreeNodePoolTest, CreateMultipleNodes) {
    auto [handle1, result1] = pool.createNode();
    auto [handle2, result2] = pool.createNode();
    auto [handle3, result3] = pool.createNode();
    
    EXPECT_EQ(result1, TreeNodePool::CreateResult::Success);
    EXPECT_EQ(result2, TreeNodePool::CreateResult::Success);
    EXPECT_EQ(result3, TreeNodePool::CreateResult::Success);
    
    EXPECT_TRUE(handle1.isValid());
    EXPECT_TRUE(handle2.isValid());
    EXPECT_TRUE(handle3.isValid());
    
    EXPECT_NE(handle1, handle2);
    EXPECT_NE(handle1, handle3);
    EXPECT_NE(handle2, handle3);
}

TEST_F(TreeNodePoolTest, TryCreateNode) {
    NodeHandle handle = pool.tryCreateNode();
    EXPECT_TRUE(handle.isValid());
    EXPECT_TRUE(pool.isValidHandle(handle));
}

TEST_F(TreeNodePoolTest, AccessNode) {
    auto [handle, result] = pool.createNode();
    
    TreeNode* node = pool.accessNode(handle);
    EXPECT_NE(node, nullptr);
    
    // Modify through access
    node->value = 42;
    node->balanceFactor = 1;
    node->height = 5;
    
    // Verify modifications
    EXPECT_EQ(node->value, 42);
    EXPECT_EQ(node->balanceFactor, 1);
    EXPECT_EQ(node->height, 5);
}

TEST_F(TreeNodePoolTest, AccessNodeModify) {
    auto [handle, result] = pool.createNode();
    
    TreeNode* node = pool.accessNode(handle);
    ASSERT_NE(node, nullptr);
    
    node->value = 100;
    node->balanceFactor = 1;
    node->height = 5;
    
    // Verify modifications through same pointer
    EXPECT_EQ(node->value, 100);
    EXPECT_EQ(node->balanceFactor, 1);
    EXPECT_EQ(node->height, 5);
}

TEST_F(TreeNodePoolTest, AccessNodeConstRead) {
    auto [handle, result] = pool.createNode();
    
    // First modify through non-const
    TreeNode* node = pool.accessNode(handle);
    ASSERT_NE(node, nullptr);
    node->value = 200;
    
    // Then read through const
    const TreeNodePool& constPool = pool;
    const TreeNode* constNode = constPool.accessNode(handle);
    EXPECT_NE(constNode, nullptr);
    EXPECT_EQ(constNode->value, 200);
    
    // Ensure const prevents modification
    //constNode->value = 300;  // This should NOT compile (good!)
}


TEST_F(TreeNodePoolTest, DeleteNode) {
    auto [handle, result] = pool.createNode();
    EXPECT_TRUE(pool.isValidHandle(handle));
    
    bool deleteResult = pool.deleteNode(handle);
    EXPECT_TRUE(deleteResult);
    EXPECT_FALSE(pool.isValidHandle(handle));
    
    TreeNode* node = pool.accessNode(handle);
    EXPECT_EQ(node, nullptr);
}

TEST_F(TreeNodePoolTest, DeleteInvalidNode) {
    NodeHandle invalidHandle;
    bool result = pool.deleteNode(invalidHandle);
    EXPECT_FALSE(result);
}

TEST_F(TreeNodePoolTest, DeleteAlreadyDeletedNode) {
    auto [handle, result] = pool.createNode();
    EXPECT_TRUE(pool.deleteNode(handle));
    
    bool secondDelete = pool.deleteNode(handle);
    EXPECT_FALSE(secondDelete);
}

TEST_F(TreeNodePoolTest, MemoryReuse) {
    auto [handle1, result1] = pool.createNode();
    size_t originalIndex = handle1.index;
    
    EXPECT_TRUE(pool.deleteNode(handle1));
    
    // Create new node - should reuse the slot
    auto [handle2, result2] = pool.createNode();
    EXPECT_EQ(result2, TreeNodePool::CreateResult::Success);
    
    // Should have same index but different generation
    EXPECT_EQ(handle2.index, originalIndex);
    EXPECT_NE(handle2.generation, handle1.generation);
    
    // Original handle should no longer be valid
    EXPECT_FALSE(pool.isValidHandle(handle1));
    EXPECT_TRUE(pool.isValidHandle(handle2));
}

TEST_F(TreeNodePoolTest, CapacityMethods) {
    EXPECT_EQ(pool.capacity(), 1000000);
    EXPECT_EQ(pool.getTotalNodeCount(), 0);
    EXPECT_EQ(pool.getActiveNodeCount(), 0);
    EXPECT_EQ(pool.getAvailableCapacity(), 1000000);
    
    auto [handle, result] = pool.createNode();
    EXPECT_EQ(result, TreeNodePool::CreateResult::Success);
    
    EXPECT_EQ(pool.getTotalNodeCount(), 1);
    EXPECT_EQ(pool.getActiveNodeCount(), 1);
    EXPECT_EQ(pool.getAvailableCapacity(), 999999);
}



TEST_F(TreeNodePoolTest, ManyNodes) {
    const int NUM_NODES = 100;
    std::vector<NodeHandle> handles;
    
    for (int i = 0; i < NUM_NODES; ++i) {
        auto [handle, result] = pool.createNode();
        EXPECT_EQ(result, TreeNodePool::CreateResult::Success);
        handles.push_back(handle);
        EXPECT_TRUE(pool.isValidHandle(handle));
    }
    
    EXPECT_EQ(pool.getActiveNodeCount(), NUM_NODES);
    
    // Delete half of them
    for (int i = 0; i < NUM_NODES / 2; ++i) {
        EXPECT_TRUE(pool.deleteNode(handles[i]));
    }
    
    EXPECT_EQ(pool.getActiveNodeCount(), NUM_NODES / 2);
    
    // Create more nodes - some should reuse deleted slots
    for (int i = 0; i < NUM_NODES / 2; ++i) {
        auto [handle, result] = pool.createNode();
        EXPECT_EQ(result, TreeNodePool::CreateResult::Success);
        EXPECT_TRUE(pool.isValidHandle(handle));
    }
    
    EXPECT_EQ(pool.getActiveNodeCount(), NUM_NODES);
}

TEST_F(TreeNodePoolTest, InvalidHandleAccess) {
    NodeHandle invalidHandle;
    TreeNode* node = pool.accessNode(invalidHandle);
    EXPECT_EQ(node, nullptr);
    
    NodeHandle outOfBoundsHandle{9999, 1};
    node = pool.accessNode(outOfBoundsHandle);
    EXPECT_EQ(node, nullptr);
}