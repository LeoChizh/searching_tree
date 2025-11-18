#include <gtest/gtest.h>
#include "Node.hpp"

class NodeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

// Basic construction tests
TEST_F(NodeTest, DefaultConstruction) {
    Node node;
    EXPECT_EQ(node.getParent(), nullptr);
    EXPECT_EQ(node.getLeft(), nullptr);
    EXPECT_EQ(node.getRight(), nullptr);
}

TEST_F(NodeTest, ParameterizedConstruction) {
    Node parent(nullptr, nullptr, nullptr, 10);
    Node left(nullptr, nullptr, nullptr, 5);
    Node right(nullptr, nullptr, nullptr, 15);
    
    Node node(&parent, &left, &right, 7);
    
    EXPECT_EQ(node.getParent(), &parent);
    EXPECT_EQ(node.getLeft(), &left);
    EXPECT_EQ(node.getRight(), &right);
}

// setLeft functionality tests
TEST_F(NodeTest, SetLeftValid) {
    Node parent(nullptr, nullptr, nullptr, 10);
    Node child(nullptr, nullptr, nullptr, 5);
    
    EXPECT_TRUE(parent.setLeft(&child));
    EXPECT_EQ(parent.getLeft(), &child);
    EXPECT_EQ(child.getParent(), &parent);
}

TEST_F(NodeTest, SetLeftReplacesExisting) {
    Node parent(nullptr, nullptr, nullptr, 10);
    Node oldChild(nullptr, nullptr, nullptr, 5);
    Node newChild(nullptr, nullptr, nullptr, 3);
    
    parent.setLeft(&oldChild);
    EXPECT_TRUE(parent.setLeft(&newChild));
    EXPECT_EQ(parent.getLeft(), &newChild);
    EXPECT_EQ(newChild.getParent(), &parent);
    EXPECT_EQ(oldChild.getParent(), nullptr);
}

TEST_F(NodeTest, SetLeftPreventsSelfReference) {
    Node node(nullptr, nullptr, nullptr, 42);
    EXPECT_FALSE(node.setLeft(&node));
    EXPECT_EQ(node.getLeft(), nullptr);
}

TEST_F(NodeTest, SetLeftPreventsDescendantCycles) {
    Node parent(nullptr, nullptr, nullptr, 10);
    Node child(nullptr, nullptr, nullptr, 5);
    Node grandchild(nullptr, nullptr, nullptr, 3);
    
    // Build: parent -> child -> grandchild
    parent.setLeft(&child);
    child.setLeft(&grandchild);
    
    // Try to create cycle: parent -> grandchild (descendant cycle)
    EXPECT_FALSE(parent.setLeft(&grandchild));
    EXPECT_EQ(parent.getLeft(), &child); // Should remain unchanged
}

TEST_F(NodeTest, SetLeftPreventsAncestorCycles) {
    Node parent(nullptr, nullptr, nullptr, 10);
    Node child(nullptr, nullptr, nullptr, 5);
    
    // Build: parent -> child
    parent.setLeft(&child);
    
    // Try to create cycle: child -> parent (ancestor cycle)
    EXPECT_FALSE(child.setLeft(&parent));
    EXPECT_EQ(child.getLeft(), nullptr);
}

TEST_F(NodeTest, SetLeftNull) {
    Node parent(nullptr, nullptr, nullptr, 10);
    Node child(nullptr, nullptr, nullptr, 5);
    
    parent.setLeft(&child);
    EXPECT_TRUE(parent.setLeft(nullptr));
    EXPECT_EQ(parent.getLeft(), nullptr);
    EXPECT_EQ(child.getParent(), nullptr);
}

// setRight functionality tests  
TEST_F(NodeTest, SetRightValid) {
    Node parent(nullptr, nullptr, nullptr, 10);
    Node child(nullptr, nullptr, nullptr, 15);
    
    EXPECT_TRUE(parent.setRight(&child));
    EXPECT_EQ(parent.getRight(), &child);
    EXPECT_EQ(child.getParent(), &parent);
}

TEST_F(NodeTest, SetRightPreventsSelfReference) {
    Node node(nullptr, nullptr, nullptr, 42);
    EXPECT_FALSE(node.setRight(&node));
    EXPECT_EQ(node.getRight(), nullptr);
}

TEST_F(NodeTest, SetRightPreventsDescendantCycles) {
    Node root(nullptr, nullptr, nullptr, 10);
    Node middle(nullptr, nullptr, nullptr, 15);
    Node leaf(nullptr, nullptr, nullptr, 20);
    
    root.setRight(&middle);
    middle.setRight(&leaf);
    
    // Try to create cycle: root -> leaf (descendant cycle)
    EXPECT_FALSE(root.setRight(&leaf));
    EXPECT_EQ(root.getRight(), &middle);
}

TEST_F(NodeTest, SetRightPreventsAncestorCycles) {
    Node parent(nullptr, nullptr, nullptr, 10);
    Node child(nullptr, nullptr, nullptr, 15);
    
    parent.setRight(&child);
    
    // Try to create cycle: child -> parent (ancestor cycle)
    EXPECT_FALSE(child.setRight(&parent));
    EXPECT_EQ(child.getRight(), nullptr);
}

// isAncestor tests
TEST_F(NodeTest, IsAncestorDirectParent) {
    Node parent(nullptr, nullptr, nullptr, 10);
    Node child(nullptr, nullptr, nullptr, 5);
    
    parent.setLeft(&child);
    EXPECT_TRUE(child.isAncestor(&parent));
    EXPECT_FALSE(parent.isAncestor(&child));
}

TEST_F(NodeTest, IsAncestorGrandParent) {
    Node grandparent(nullptr, nullptr, nullptr, 20);
    Node parent(nullptr, nullptr, nullptr, 10);
    Node child(nullptr, nullptr, nullptr, 5);
    
    grandparent.setLeft(&parent);
    parent.setLeft(&child);
    
    EXPECT_TRUE(child.isAncestor(&grandparent));
    EXPECT_TRUE(child.isAncestor(&parent));
    EXPECT_FALSE(parent.isAncestor(&child));
    EXPECT_FALSE(grandparent.isAncestor(&child));
}

TEST_F(NodeTest, IsAncestorSelf) {
    Node node(nullptr, nullptr, nullptr, 42);
    EXPECT_TRUE(node.isAncestor(&node));
}

TEST_F(NodeTest, IsAncestorFalse) {
    Node node1(nullptr, nullptr, nullptr, 1);
    Node node2(nullptr, nullptr, nullptr, 2);
    
    EXPECT_FALSE(node1.isAncestor(&node2));
    EXPECT_FALSE(node2.isAncestor(&node1));
}

TEST_F(NodeTest, IsAncestorNull) {
    Node node(nullptr, nullptr, nullptr, 42);
    EXPECT_FALSE(node.isAncestor(nullptr));
}

// removeChild tests
TEST_F(NodeTest, RemoveLeftChild) {
    Node parent(nullptr, nullptr, nullptr, 10);
    Node child(nullptr, nullptr, nullptr, 5);
    
    parent.setLeft(&child);
    EXPECT_TRUE(parent.removeChild(&child));
    EXPECT_EQ(parent.getLeft(), nullptr);
}

TEST_F(NodeTest, RemoveRightChild) {
    Node parent(nullptr, nullptr, nullptr, 10);
    Node child(nullptr, nullptr, nullptr, 15);
    
    parent.setRight(&child);
    EXPECT_TRUE(parent.removeChild(&child));
    EXPECT_EQ(parent.getRight(), nullptr);
}

TEST_F(NodeTest, RemoveNonExistentChild) {
    Node parent(nullptr, nullptr, nullptr, 10);
    Node child(nullptr, nullptr, nullptr, 5);
    Node otherChild(nullptr, nullptr, nullptr, 7);
    
    parent.setLeft(&child);
    EXPECT_FALSE(parent.removeChild(&otherChild));
    EXPECT_EQ(parent.getLeft(), &child);
}

TEST_F(NodeTest, RemoveNullChild) {
    Node parent(nullptr, nullptr, nullptr, 10);
    EXPECT_FALSE(parent.removeChild(nullptr));
}

// Complex scenario tests
TEST_F(NodeTest, ReassignChildToNewParent) {
    Node parent1(nullptr, nullptr, nullptr, 10);
    Node parent2(nullptr, nullptr, nullptr, 20);
    Node child(nullptr, nullptr, nullptr, 5);
    
    parent1.setLeft(&child);
    EXPECT_EQ(child.getParent(), &parent1);
    
    parent2.setRight(&child);
    EXPECT_EQ(child.getParent(), &parent2);
    EXPECT_EQ(parent1.getLeft(), nullptr);
}

TEST_F(NodeTest, TreeStructureIntegrity) {
    Node root(nullptr, nullptr, nullptr, 50);
    Node leftChild(nullptr, nullptr, nullptr, 25);
    Node rightChild(nullptr, nullptr, nullptr, 75); 
    Node leftGrandchild(nullptr, nullptr, nullptr, 10);
    
    EXPECT_TRUE(root.setLeft(&leftChild));
    EXPECT_TRUE(root.setRight(&rightChild));
    EXPECT_TRUE(leftChild.setLeft(&leftGrandchild));
    
    EXPECT_EQ(root.getLeft(), &leftChild);
    EXPECT_EQ(root.getRight(), &rightChild);
    EXPECT_EQ(leftChild.getLeft(), &leftGrandchild);
    
    EXPECT_EQ(leftChild.getParent(), &root);
    EXPECT_EQ(rightChild.getParent(), &root);
    EXPECT_EQ(leftGrandchild.getParent(), &leftChild);
}

TEST_F(NodeTest, ComplexCycleDetection) {
    Node node1(nullptr, nullptr, nullptr, 1);
    Node node2(nullptr, nullptr, nullptr, 2);
    Node node3(nullptr, nullptr, nullptr, 3);
    Node node4(nullptr, nullptr, nullptr, 4);
    
    // Build chain: node1 -> node2 -> node3 -> node4
    node1.setLeft(&node2);
    node2.setLeft(&node3);
    node3.setLeft(&node4);
    
    // All cycle attempts should fail
    EXPECT_FALSE(node1.setLeft(&node3));
    EXPECT_FALSE(node1.setRight(&node4));
    EXPECT_FALSE(node2.setLeft(&node4));
    
    // Reverse cycles should also fail
    EXPECT_FALSE(node4.setLeft(&node1));
    EXPECT_FALSE(node4.setLeft(&node2));
    EXPECT_FALSE(node4.setLeft(&node3));
    EXPECT_FALSE(node3.setLeft(&node1));
    EXPECT_FALSE(node3.setLeft(&node2));
}

TEST_F(NodeTest, MultipleParentChanges) {
    Node node(nullptr, nullptr, nullptr, 5);
    Node parent1(nullptr, nullptr, nullptr, 10);
    Node parent2(nullptr, nullptr, nullptr, 15);
    Node parent3(nullptr, nullptr, nullptr, 20);
    
    EXPECT_TRUE(parent1.setLeft(&node));
    EXPECT_EQ(node.getParent(), &parent1);
    
    EXPECT_TRUE(parent2.setRight(&node));
    EXPECT_EQ(node.getParent(), &parent2);
    EXPECT_EQ(parent1.getLeft(), nullptr);
    
    EXPECT_TRUE(parent3.setLeft(&node));
    EXPECT_EQ(node.getParent(), &parent3);
    EXPECT_EQ(parent2.getRight(), nullptr);
}

// Edge case tests
TEST_F(NodeTest, SetChildThatIsAlreadyChild) {
    Node parent(nullptr, nullptr, nullptr, 10);
    Node child(nullptr, nullptr, nullptr, 5);
    
    parent.setLeft(&child);
    // Setting same child should work (no-op)
    EXPECT_FALSE(parent.setLeft(&child));
    EXPECT_EQ(parent.getLeft(), &child);
    EXPECT_EQ(child.getParent(), &parent);
}

TEST_F(NodeTest, CircularReferencePrevention) {
    Node node1(nullptr, nullptr, nullptr, 1);
    Node node2(nullptr, nullptr, nullptr, 2);
    
    // Should not be able to create any cycles
    EXPECT_TRUE(node1.setLeft(&node2));
    EXPECT_FALSE(node2.setLeft(&node1)); // Cycle prevention
    
    EXPECT_EQ(node1.getLeft(), &node2);
    EXPECT_EQ(node2.getLeft(), nullptr); // Cycle was prevented
    EXPECT_EQ(node2.getParent(), &node1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}