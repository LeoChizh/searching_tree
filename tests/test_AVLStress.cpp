#include <gtest/gtest.h>
#include <random>
#include <chrono>
#include <algorithm>
#include <numeric>
#include "AVLTree.hpp"

class AVLTreeStressTest : public ::testing::Test {
protected:
    void SetUp() override {
        tree = std::make_unique<AVLTree>();
    }

    void TearDown() override {
        tree.reset();
    }

    std::unique_ptr<AVLTree> tree;
};

TEST_F(AVLTreeStressTest, LargeSequentialInsertion) {
    const int NUM_NODES = 100000;
    
    for (int i = 0; i < NUM_NODES; ++i) {
        EXPECT_TRUE(tree->insert(i));
    }
    
    EXPECT_EQ(tree->size(), NUM_NODES);
    
    // Verify all values are present
    for (int i = 0; i < NUM_NODES; i += 1000) {
        EXPECT_TRUE(tree->contains(i));
    }
    
    // Tree should be balanced (height should be logarithmic)
    int height = tree->getHeight();
    int maxExpectedHeight = 2 * static_cast<int>(std::log2(NUM_NODES + 1));
    EXPECT_LE(height, maxExpectedHeight);
    
    std::cout << "Large sequential tree - Size: " << tree->size() 
              << ", Height: " << height 
              << ", Max expected: " << maxExpectedHeight << std::endl;
}

TEST_F(AVLTreeStressTest, LargeRandomInsertion) {
    const int NUM_NODES = 100000;
    std::vector<int> values;
    
    // Generate random values
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, NUM_NODES * 10);
    
    int successful_insertions = 0;
    for (int i = 0; i < NUM_NODES; ++i) {
        int value = dis(gen);
        if (tree->insert(value)) {
            successful_insertions++;
            values.push_back(value);
        }
    }
    
    // Test should check the actual number of successful insertions
    EXPECT_EQ(tree->size(), successful_insertions);
    
    // Verify random sample of successfully inserted values
    if (successful_insertions > 0) {
        std::uniform_int_distribution<> index_dis(0, successful_insertions - 1);
        for (int i = 0; i < 100; ++i) {
            int index = index_dis(gen);
            EXPECT_TRUE(tree->contains(values[index]));
        }
    }
    
    int height = tree->getHeight();
    int maxExpectedHeight = 2 * static_cast<int>(std::log2(successful_insertions + 1));
    EXPECT_LE(height, maxExpectedHeight);
    
    std::cout << "Large random tree - Unique values: " << successful_insertions 
              << "/" << NUM_NODES << " (" << (successful_insertions * 100.0 / NUM_NODES) << "%)"
              << ", Height: " << height << std::endl;
}

TEST_F(AVLTreeStressTest, LargeUniqueRandomInsertion) {
    const int NUM_NODES = 100000;
    std::vector<int> values(NUM_NODES);
    
    // Generate unique random values using iota + shuffle
    std::iota(values.begin(), values.end(), 1); // 1, 2, 3, ..., NUM_NODES
    std::shuffle(values.begin(), values.end(), std::mt19937{std::random_device{}()});
    
    for (int value : values) {
        EXPECT_TRUE(tree->insert(value));
    }
    
    EXPECT_EQ(tree->size(), NUM_NODES);
    
    // Verify all values are present
    for (int i = 0; i < 100; ++i) {
        int index = std::rand() % NUM_NODES;
        EXPECT_TRUE(tree->contains(values[index]));
    }
    
    int height = tree->getHeight();
    int maxExpectedHeight = 2 * static_cast<int>(std::log2(NUM_NODES + 1));
    EXPECT_LE(height, maxExpectedHeight);
    
    std::cout << "Large unique random tree - Size: " << tree->size() 
              << ", Height: " << height << std::endl;
}

TEST_F(AVLTreeStressTest, MixedOperationsStressTest) {
    const int OPERATIONS = 50000;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> op_dis(0, 2);
    std::uniform_int_distribution<> val_dis(1, 100000);
    
    int insertions = 0;
    int deletions = 0;
    
    for (int i = 0; i < OPERATIONS; ++i) {
        int operation = op_dis(gen);
        int value = val_dis(gen);
        
        switch (operation) {
            case 0: // Insert
                if (tree->insert(value)) {
                    insertions++;
                }
                break;
            case 1: // Remove
                if (tree->remove(value)) {
                    deletions++;
                }
                break;
            case 2: // Contains
                tree->contains(value);
                break;
        }
        
        // Periodically check tree integrity
        if (i % 10000 == 0) {
            int height = tree->getHeight();
            int size = tree->size();
            std::cout << "Operation " << i << " - Size: " << size 
                      << ", Height: " << height << std::endl;
            
            // Height should be reasonable for current size
            if (size > 0) {
                int maxExpectedHeight = 2 * static_cast<int>(std::log2(size + 1));
                EXPECT_LE(height, maxExpectedHeight + 5); // Allow some tolerance
            }
        }
    }
    
    std::cout << "Mixed operations completed - Final size: " << tree->size() 
              << ", Insertions: " << insertions << ", Deletions: " << deletions << std::endl;
}

TEST_F(AVLTreeStressTest, QueryPerformanceLargeTree) {
    const int NUM_NODES = 100000;
    
    // Build large tree
    for (int i = 0; i < NUM_NODES; ++i) {
        tree->insert(i * 2); // Even numbers only
    }
    
    // Test countSmallerThan performance
    auto start = std::chrono::high_resolution_clock::now();
    
    [[maybe_unused]]size_t count = tree->countSmallerThan(50000);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "countSmallerThan(50000) on " << NUM_NODES 
              << " nodes took: " << duration.count() << " microseconds" << std::endl;
    
    // Test findNthSmallest performance
    start = std::chrono::high_resolution_clock::now();
    auto result = tree->findNthSmallest(50000);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "findNthSmallest(50000) on " << NUM_NODES 
              << " nodes took: " << duration.count() << " microseconds" << std::endl;
    
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 99998); // 50000th smallest in 0,2,4,... should be 99998
}

TEST_F(AVLTreeStressTest, ClearLargeTree) {
    const int NUM_NODES = 100000;
    
    // Build large tree
    for (int i = 0; i < NUM_NODES; ++i) {
        tree->insert(i);
    }
    
    EXPECT_EQ(tree->size(), NUM_NODES);
    
    auto start = std::chrono::high_resolution_clock::now();
    tree->clear();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Clearing " << NUM_NODES << " nodes took: " 
              << duration.count() << " milliseconds" << std::endl;
    
    EXPECT_TRUE(tree->empty());
    EXPECT_EQ(tree->size(), 0);
}

TEST_F(AVLTreeStressTest, CopyLargeTree) {
    const int NUM_NODES = 50000;
    
    // Build large tree
    for (int i = 0; i < NUM_NODES; ++i) {
        tree->insert(i);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    AVLTree copy(*tree);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Copying " << NUM_NODES << " nodes took: " 
              << duration.count() << " milliseconds" << std::endl;
    
    EXPECT_EQ(copy.size(), NUM_NODES);
    EXPECT_EQ(tree->size(), NUM_NODES);
    
    // Verify copy contains same values
    for (int i = 0; i < NUM_NODES; i += 1000) {
        EXPECT_TRUE(copy.contains(i));
    }
}

// Simple fuzz test without complex scope issues
TEST_F(AVLTreeStressTest, SimpleFuzzTest) {
    const int OPERATIONS = 10000;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> op_dis(0, 1); // Only insert/remove
    std::uniform_int_distribution<> val_dis(1, 1000);
    
    for (int i = 0; i < OPERATIONS; ++i) {
        int operation = op_dis(gen);
        int value = val_dis(gen);
        
        if (operation == 0) {
            tree->insert(value);
        } else {
            tree->remove(value);
        }
        
        // Basic sanity check - tree should remain consistent
        if (tree->size() > 0) {
            auto first = tree->findNthSmallest(1);
            EXPECT_TRUE(first.has_value());
            
            auto last = tree->findNthSmallest(tree->size());
            EXPECT_TRUE(last.has_value());
            
            EXPECT_LE(first.value(), last.value());
        }
    }
    
    std::cout << "Fuzz test completed - Final size: " << tree->size() << std::endl;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}