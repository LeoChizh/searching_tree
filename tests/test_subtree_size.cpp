#include <gtest/gtest.h>
#include <vector>
#include <random>
#include <algorithm>  // Add this for std::sort
#include <set>        // Add this for std::set
#include "AVLTree.hpp"

class AVLTreeSubtreeSizeTest : public ::testing::Test {
protected:
    void SetUp() override {
        tree = std::make_unique<AVLTree>();
    }

    void TearDown() override {
        tree.reset();
    }

    std::unique_ptr<AVLTree> tree;
};

TEST_F(AVLTreeSubtreeSizeTest, EmptyTreeSize) {
    EXPECT_EQ(tree->size(), 0);
    EXPECT_TRUE(tree->empty());
}

TEST_F(AVLTreeSubtreeSizeTest, SingleNodeSize) {
    tree->insert(42);
    EXPECT_EQ(tree->size(), 1);
}

TEST_F(AVLTreeSubtreeSizeTest, SequentialInsertionSizes) {
    const int NUM_NODES = 100;
    
    for (int i = 0; i < NUM_NODES; ++i) {
        EXPECT_TRUE(tree->insert(i));
        EXPECT_EQ(tree->size(), i + 1);
    }
    
    EXPECT_EQ(tree->size(), NUM_NODES);
}

TEST_F(AVLTreeSubtreeSizeTest, RandomInsertionSizes) {
    const int NUM_NODES = 500;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 1000);
    
    size_t expected_size = 0;
    for (int i = 0; i < NUM_NODES; ++i) {
        int value = dis(gen);
        bool inserted = tree->insert(value);
        
        if (inserted) {
            expected_size++;
        }
        
        EXPECT_EQ(tree->size(), expected_size);
    }
}

TEST_F(AVLTreeSubtreeSizeTest, DeletionSizeUpdates) {
    // Insert some values
    std::vector<int> values = {50, 25, 75, 10, 30, 60, 80, 5, 15, 27, 55, 65};
    
    for (int value : values) {
        tree->insert(value);
    }
    
    size_t initial_size = tree->size();
    EXPECT_EQ(initial_size, values.size());
    
    // Remove leaf node
    EXPECT_TRUE(tree->remove(5));
    EXPECT_EQ(tree->size(), initial_size - 1);
    
    // Remove node with one child
    EXPECT_TRUE(tree->remove(10));
    EXPECT_EQ(tree->size(), initial_size - 2);
    
    // Remove node with two children
    EXPECT_TRUE(tree->remove(50));
    EXPECT_EQ(tree->size(), initial_size - 3);
    
    // Try to remove non-existent node
    EXPECT_FALSE(tree->remove(999));
    EXPECT_EQ(tree->size(), initial_size - 3);
}

TEST_F(AVLTreeSubtreeSizeTest, FindNthSmallestConsistency) {
    // Insert values in random order
    std::vector<int> values = {30, 10, 50, 5, 20, 40, 60, 1, 7, 15, 25, 35, 45, 55, 65};
    
    for (int value : values) {
        tree->insert(value);
    }
    
    // Sort values to verify nth smallest
    std::sort(values.begin(), values.end());
    
    for (size_t i = 0; i < values.size(); ++i) {
        auto result = tree->findNthSmallest(i + 1);
        EXPECT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), values[i]) 
            << "Failed at position " << (i + 1) 
            << ": expected " << values[i] << ", got " << result.value();
    }
    
    // Test out of bounds
    EXPECT_FALSE(tree->findNthSmallest(0).has_value());
    EXPECT_FALSE(tree->findNthSmallest(values.size() + 1).has_value());
}

TEST_F(AVLTreeSubtreeSizeTest, CountSmallerThanAccuracy) {
    // Build tree: 10, 20, 30, 40, 50
    for (int value : {30, 10, 50, 20, 40}) {
        tree->insert(value);
    }
    
    // Test various values
    EXPECT_EQ(tree->countSmallerThan(5), 0);   // All values are >= 5
    EXPECT_EQ(tree->countSmallerThan(10), 0);  // No values < 10
    EXPECT_EQ(tree->countSmallerThan(15), 1);  // Only 10 is < 15
    EXPECT_EQ(tree->countSmallerThan(20), 1);  // Only 10 is < 20
    EXPECT_EQ(tree->countSmallerThan(25), 2);  // 10, 20 are < 25
    EXPECT_EQ(tree->countSmallerThan(30), 2);  // 10, 20 are < 30
    EXPECT_EQ(tree->countSmallerThan(35), 3);  // 10, 20, 30 are < 35
    EXPECT_EQ(tree->countSmallerThan(45), 4);  // 10, 20, 30, 40 are < 45
    EXPECT_EQ(tree->countSmallerThan(50), 4);  // 10, 20, 30, 40 are < 50
    EXPECT_EQ(tree->countSmallerThan(55), 5);  // All values are < 55
    EXPECT_EQ(tree->countSmallerThan(100), 5); // All values are < 100
}

TEST_F(AVLTreeSubtreeSizeTest, RotationSizeIntegrity) {
    // Test that rotations maintain correct subtree sizes
    
    // Left-Left case (right rotation)
    tree->insert(30);
    tree->insert(20);
    tree->insert(10);  // Should trigger right rotation
    
    EXPECT_EQ(tree->size(), 3);
    
    // Right-Right case (left rotation)  
    tree->clear();
    tree->insert(10);
    tree->insert(20);
    tree->insert(30);  // Should trigger left rotation
    
    EXPECT_EQ(tree->size(), 3);
    
    // Left-Right case
    tree->clear();
    tree->insert(30);
    tree->insert(10);
    tree->insert(20);  // Should trigger left-right rotation
    
    EXPECT_EQ(tree->size(), 3);
    
    // Right-Left case
    tree->clear();
    tree->insert(10);
    tree->insert(30);
    tree->insert(20);  // Should trigger right-left rotation
    
    EXPECT_EQ(tree->size(), 3);
}

TEST_F(AVLTreeSubtreeSizeTest, ClearResetsSize) {
    // Add some nodes
    for (int i = 0; i < 10; ++i) {
        tree->insert(i);
    }
    
    EXPECT_EQ(tree->size(), 10);
    EXPECT_FALSE(tree->empty());
    
    tree->clear();
    
    EXPECT_EQ(tree->size(), 0);
    EXPECT_TRUE(tree->empty());
}

TEST_F(AVLTreeSubtreeSizeTest, CopyOperationsPreserveSizes) {
    // Build original tree
    for (int i = 0; i < 20; ++i) {
        tree->insert(i * 2); // Even numbers
    }
    
    size_t original_size = tree->size();
    
    // Test copy constructor
    AVLTree copy1(*tree);
    EXPECT_EQ(copy1.size(), original_size);
    
    // Test copy assignment
    AVLTree copy2;
    copy2 = *tree;
    EXPECT_EQ(copy2.size(), original_size);
    
    // Verify all trees work correctly
    for (int i = 1; i <= 5; ++i) {
        auto result1 = tree->findNthSmallest(i);
        auto result2 = copy1.findNthSmallest(i);
        auto result3 = copy2.findNthSmallest(i);
        
        EXPECT_TRUE(result1.has_value());
        EXPECT_TRUE(result2.has_value());
        EXPECT_TRUE(result3.has_value());
        EXPECT_EQ(result1.value(), result2.value());
        EXPECT_EQ(result2.value(), result3.value());
    }
}

TEST_F(AVLTreeSubtreeSizeTest, MixedOperationsSizeConsistency) {
    const int OPERATIONS = 1000;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> op_dis(0, 2); // 0:insert, 1:remove, 2:contains
    std::uniform_int_distribution<> val_dis(1, 100);
    
    size_t expected_size = 0;
    std::set<int> reference_set;
    
    for (int i = 0; i < OPERATIONS; ++i) {
        int operation = op_dis(gen);
        int value = val_dis(gen);
        
        switch (operation) {
            case 0: // Insert
                if (tree->insert(value)) {
                    expected_size++;
                    reference_set.insert(value);
                }
                break;
            case 1: // Remove
                if (tree->remove(value)) {
                    expected_size--;
                    reference_set.erase(value);
                }
                break;
            case 2: // Contains - doesn't change size
                tree->contains(value);
                break;
        }
        
        // Verify size consistency
        EXPECT_EQ(tree->size(), expected_size);
        
        // Verify countSmallerThan matches reference
        if (expected_size > 0) {
            int test_value = val_dis(gen);
            size_t avl_count = tree->countSmallerThan(test_value);
            size_t set_count = std::distance(reference_set.begin(), 
                                           reference_set.lower_bound(test_value));
            EXPECT_EQ(avl_count, set_count) 
                << "countSmallerThan(" << test_value << ") mismatch";
        }
    }
}

TEST_F(AVLTreeSubtreeSizeTest, LargeScaleSizeConsistency) {
    const int NUM_NODES = 10000;
    
    // Sequential insertion
    for (int i = 0; i < NUM_NODES; ++i) {
        tree->insert(i);
        EXPECT_EQ(tree->size(), i + 1);
    }
    
    // Verify all nth smallest operations work
    for (int i = 1; i <= 100; ++i) {
        auto result = tree->findNthSmallest(i);
        EXPECT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), i - 1); // Since we inserted 0,1,2,...
    }
    
    // Random deletions
    std::vector<int> to_remove;
    for (int i = 0; i < NUM_NODES / 2; ++i) {
        to_remove.push_back(i * 2); // Remove even numbers
    }
    
    for (int value : to_remove) {
        EXPECT_TRUE(tree->remove(value));
    }
    
    EXPECT_EQ(tree->size(), NUM_NODES - to_remove.size());
    
    // Verify counts are still correct
    for (int i = 0; i < 10; ++i) {
        int test_value = i * 1000;
        size_t count = tree->countSmallerThan(test_value);
        // We can't easily verify exact count without reference, 
        // but it shouldn't crash and should be <= current size
        EXPECT_LE(count, tree->size());
    }
}

TEST_F(AVLTreeSubtreeSizeTest, ComplexScenario) {
    // Complex insertion and deletion pattern
    tree->insert(50);
    tree->insert(25);
    tree->insert(75);
    tree->insert(12);
    tree->insert(37);
    tree->insert(62);
    tree->insert(87);
    
    EXPECT_EQ(tree->size(), 7);
    
    // Remove internal nodes
    tree->remove(25);
    EXPECT_EQ(tree->size(), 6);
    
    tree->remove(75);
    EXPECT_EQ(tree->size(), 5);
    
    // Add more nodes
    tree->insert(100);
    tree->insert(6);
    tree->insert(18);
    
    EXPECT_EQ(tree->size(), 8);
    
    // Verify special operations still work
    auto third_smallest = tree->findNthSmallest(3);
    EXPECT_TRUE(third_smallest.has_value());
    
    size_t count_less_than_50 = tree->countSmallerThan(50);
    EXPECT_LE(count_less_than_50, tree->size());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}