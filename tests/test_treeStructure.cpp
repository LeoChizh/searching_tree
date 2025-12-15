#include <gtest/gtest.h>
#include "TreeStructure.hpp"
#include "TreeNodePool.hpp"
#include "TreeResourceHandle.hpp"

using NodeHandle = TreeResourceHandle;

class TreeStructureTest : public ::testing::Test {
protected:
    void SetUp() override {
        pool = std::make_unique<TreeNodePool>();
    }

    void TearDown() override {
        pool.reset();
    }

    std::unique_ptr<TreeNodePool> pool;
};

TEST_F(TreeStructureTest, MemoryExhaustion) {
    TreeNodePool pool(1);  // Only 1 node capacity
    auto [h1, r1] = pool.createNode();
    EXPECT_EQ(r1, TreeNodePool::CreateResult::Success);
    
    // This should handle failure gracefully
    auto [h2, r2] = pool.createNode();
    EXPECT_EQ(r2, TreeNodePool::CreateResult::TooManyNodes);
    
    // Your noexcept functions might terminate here if they throw
}

TEST_F(TreeStructureTest, InvalidHandleAccess) {
    TreeNodePool pool;
    NodeHandle invalidHandle{999999, 999999};  // Way out of bounds
    
    // This should not terminate
    bool result = TreeStructure::setNodeValue(pool, invalidHandle, 42);
    EXPECT_FALSE(result);  // Should return false, not terminate
}

TEST_F(TreeStructureTest, SetLeftChild) {
    auto [parentHandle, parentResult] = pool->createNode();
    auto [childHandle, childResult] = pool->createNode();
    
    bool result = TreeStructure::setLeftChild(*pool, parentHandle, childHandle);
    EXPECT_TRUE(result);
    
    NodeHandle retrievedChild = TreeStructure::getLeftChild(*pool, parentHandle);
    EXPECT_EQ(retrievedChild, childHandle);
    
    NodeHandle retrievedParent = TreeStructure::getParent(*pool, childHandle);
    EXPECT_EQ(retrievedParent, parentHandle);
}

TEST_F(TreeStructureTest, SetRightChild) {
    auto [parentHandle, parentResult] = pool->createNode();
    auto [childHandle, childResult] = pool->createNode();
    
    bool result = TreeStructure::setRightChild(*pool, parentHandle, childHandle);
    EXPECT_TRUE(result);
    
    NodeHandle retrievedChild = TreeStructure::getRightChild(*pool, parentHandle);
    EXPECT_EQ(retrievedChild, childHandle);
    
    NodeHandle retrievedParent = TreeStructure::getParent(*pool, childHandle);
    EXPECT_EQ(retrievedParent, parentHandle);
}

TEST_F(TreeStructureTest, SetLeftChildInvalidParent) {
    NodeHandle invalidParent;
    auto [childHandle, childResult] = pool->createNode();
    
    bool result = TreeStructure::setLeftChild(*pool, invalidParent, childHandle);
    EXPECT_FALSE(result);
}

TEST_F(TreeStructureTest, SetRightChildInvalidParent) {
    NodeHandle invalidParent;
    auto [childHandle, childResult] = pool->createNode();
    
    bool result = TreeStructure::setRightChild(*pool, invalidParent, childHandle);
    EXPECT_FALSE(result);
}

TEST_F(TreeStructureTest, SetLeftChildInvalidChild) {
    auto [parentHandle, parentResult] = pool->createNode();
    NodeHandle invalidChild;
    
    bool result = TreeStructure::setLeftChild(*pool, parentHandle, invalidChild);
    EXPECT_TRUE(result); // Should succeed - invalid child means remove left child
    
    NodeHandle retrievedChild = TreeStructure::getLeftChild(*pool, parentHandle);
    EXPECT_FALSE(retrievedChild.isValid());
}

TEST_F(TreeStructureTest, SetRightChildInvalidChild) {
    auto [parentHandle, parentResult] = pool->createNode();
    NodeHandle invalidChild;
    
    bool result = TreeStructure::setRightChild(*pool, parentHandle, invalidChild);
    EXPECT_TRUE(result); // Should succeed - invalid child means remove right child
    
    NodeHandle retrievedChild = TreeStructure::getRightChild(*pool, parentHandle);
    EXPECT_FALSE(retrievedChild.isValid());
}

TEST_F(TreeStructureTest, SetNodeValue) {
    auto [handle, result] = pool->createNode();
    
    bool setResult = TreeStructure::setNodeValue(*pool, handle, 42);
    EXPECT_TRUE(setResult);
    
    auto value = TreeStructure::getNodeValue(*pool, handle);
    EXPECT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), 42);
}

TEST_F(TreeStructureTest, SetBalanceFactor) {
    auto [handle, result] = pool->createNode();
    
    bool setResult = TreeStructure::setBalanceFactor(*pool, handle, -1);
    EXPECT_TRUE(setResult);
    
    auto balance = TreeStructure::getBalanceFactor(*pool, handle);
    EXPECT_TRUE(balance.has_value());
    EXPECT_EQ(balance.value(), -1);
}

TEST_F(TreeStructureTest, SetHeight) {
    auto [handle, result] = pool->createNode();
    
    bool setResult = TreeStructure::setHeight(*pool, handle, 10);
    EXPECT_TRUE(setResult);
    
    auto height = TreeStructure::getHeight(*pool, handle);
    EXPECT_TRUE(height.has_value());
    EXPECT_EQ(height.value(), 10);
}

TEST_F(TreeStructureTest, SetPropertiesInvalidHandle) {
    NodeHandle invalidHandle;
    
    EXPECT_FALSE(TreeStructure::setNodeValue(*pool, invalidHandle, 42));
    EXPECT_FALSE(TreeStructure::setBalanceFactor(*pool, invalidHandle, 1));
    EXPECT_FALSE(TreeStructure::setHeight(*pool, invalidHandle, 5));
    
    auto value = TreeStructure::getNodeValue(*pool, invalidHandle);
    EXPECT_FALSE(value.has_value());
    
    auto balance = TreeStructure::getBalanceFactor(*pool, invalidHandle);
    EXPECT_FALSE(balance.has_value());
    
    auto height = TreeStructure::getHeight(*pool, invalidHandle);
    EXPECT_FALSE(height.has_value());
}

TEST_F(TreeStructureTest, GetChildrenInvalidHandle) {
    NodeHandle invalidHandle;
    
    NodeHandle left = TreeStructure::getLeftChild(*pool, invalidHandle);
    NodeHandle right = TreeStructure::getRightChild(*pool, invalidHandle);
    NodeHandle parent = TreeStructure::getParent(*pool, invalidHandle);
    
    EXPECT_FALSE(left.isValid());
    EXPECT_FALSE(right.isValid());
    EXPECT_FALSE(parent.isValid());
}

TEST_F(TreeStructureTest, ComplexTreeStructure) {
    // Build: root -> (left, right), left -> (leftLeft, leftRight)
    auto [rootHandle, _] = pool->createNode();
    auto [leftHandle, __] = pool->createNode();
    auto [rightHandle, ___] = pool->createNode();
    auto [leftLeftHandle, ____] = pool->createNode();
    auto [leftRightHandle, _____] = pool->createNode();
    
    // Set up relationships
    EXPECT_TRUE(TreeStructure::setLeftChild(*pool, rootHandle, leftHandle));
    EXPECT_TRUE(TreeStructure::setRightChild(*pool, rootHandle, rightHandle));
    EXPECT_TRUE(TreeStructure::setLeftChild(*pool, leftHandle, leftLeftHandle));
    EXPECT_TRUE(TreeStructure::setRightChild(*pool, leftHandle, leftRightHandle));
    
    // Verify relationships
    EXPECT_EQ(TreeStructure::getLeftChild(*pool, rootHandle), leftHandle);
    EXPECT_EQ(TreeStructure::getRightChild(*pool, rootHandle), rightHandle);
    EXPECT_EQ(TreeStructure::getLeftChild(*pool, leftHandle), leftLeftHandle);
    EXPECT_EQ(TreeStructure::getRightChild(*pool, leftHandle), leftRightHandle);
    
    EXPECT_EQ(TreeStructure::getParent(*pool, leftHandle), rootHandle);
    EXPECT_EQ(TreeStructure::getParent(*pool, rightHandle), rootHandle);
    EXPECT_EQ(TreeStructure::getParent(*pool, leftLeftHandle), leftHandle);
    EXPECT_EQ(TreeStructure::getParent(*pool, leftRightHandle), leftHandle);
}

TEST_F(TreeStructureTest, UpdateChildRelationship) {
    auto [parentHandle, _] = pool->createNode();
    auto [firstChildHandle, __] = pool->createNode();
    auto [secondChildHandle, ___] = pool->createNode();
    
    // Set first child
    EXPECT_TRUE(TreeStructure::setLeftChild(*pool, parentHandle, firstChildHandle));
    EXPECT_EQ(TreeStructure::getLeftChild(*pool, parentHandle), firstChildHandle);
    EXPECT_EQ(TreeStructure::getParent(*pool, firstChildHandle), parentHandle);
    
    // Replace with second child
    EXPECT_TRUE(TreeStructure::setLeftChild(*pool, parentHandle, secondChildHandle));
    EXPECT_EQ(TreeStructure::getLeftChild(*pool, parentHandle), secondChildHandle);
    EXPECT_EQ(TreeStructure::getParent(*pool, secondChildHandle), parentHandle);
    
    // First child should no longer have parent
    EXPECT_FALSE(TreeStructure::getParent(*pool, firstChildHandle).isValid());
}

TEST_F(TreeStructureTest, TreeOperationsAfterNodeDeletion) {
    auto [parentHandle, _] = pool->createNode();
    auto [childHandle, __] = pool->createNode();
    
    EXPECT_TRUE(TreeStructure::setLeftChild(*pool, parentHandle, childHandle));
    
    // Delete child node
    EXPECT_TRUE(pool->deleteNode(childHandle));
    
    // Parent should still have reference to deleted child
    NodeHandle retrievedChild = TreeStructure::getLeftChild(*pool, parentHandle);
    EXPECT_TRUE(retrievedChild.isValid()); // Handle is still valid structurally
    EXPECT_FALSE(pool->isValidHandle(retrievedChild)); // But node is gone
    
    // Operations on deleted child should fail
    EXPECT_FALSE(TreeStructure::setNodeValue(*pool, childHandle, 42));
    auto value = TreeStructure::getNodeValue(*pool, childHandle);
    EXPECT_FALSE(value.has_value());
}