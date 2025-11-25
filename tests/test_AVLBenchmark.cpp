#include <gtest/gtest.h>
#include <random>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <set>
#include <vector>
#include "AVLTree.hpp"

class AVLTreeBenchmark : public ::testing::Test {
protected:
    void SetUp() override {
        avl_tree = std::make_unique<AVLTree>();
        std_set = std::make_unique<std::set<int>>();
    }

    void TearDown() override {
        avl_tree.reset();
        std_set.reset();
    }

    std::unique_ptr<AVLTree> avl_tree;
    std::unique_ptr<std::set<int>> std_set;
    
    // Common test data
    const int NUM_ELEMENTS = 100000;
    std::vector<int> sequential_data;
    std::vector<int> random_data;
    std::vector<int> unique_random_data;

    void generateTestData() {
        // Sequential data
        sequential_data.resize(NUM_ELEMENTS);
        std::iota(sequential_data.begin(), sequential_data.end(), 0);
        
        // Random data with duplicates
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, NUM_ELEMENTS * 2);
        random_data.resize(NUM_ELEMENTS);
        for (auto& val : random_data) {
            val = dis(gen);
        }
        
        // Unique random data
        unique_random_data.resize(NUM_ELEMENTS);
        std::iota(unique_random_data.begin(), unique_random_data.end(), 0);
        std::shuffle(unique_random_data.begin(), unique_random_data.end(), gen);
    }
};

TEST_F(AVLTreeBenchmark, SequentialInsertionBenchmark) {
    generateTestData();
    
    // AVL Tree insertion
    auto avl_start = std::chrono::high_resolution_clock::now();
    for (int value : sequential_data) {
        avl_tree->insert(value);
    }
    auto avl_end = std::chrono::high_resolution_clock::now();
    auto avl_duration = std::chrono::duration_cast<std::chrono::milliseconds>(avl_end - avl_start);
    
    // std::set insertion
    auto set_start = std::chrono::high_resolution_clock::now();
    for (int value : sequential_data) {
        std_set->insert(value);
    }
    auto set_end = std::chrono::high_resolution_clock::now();
    auto set_duration = std::chrono::duration_cast<std::chrono::milliseconds>(set_end - set_start);
    
    std::cout << "\n=== Sequential Insertion Benchmark ===" << std::endl;
    std::cout << "AVL Tree: " << avl_duration.count() << " ms" << std::endl;
    std::cout << "std::set: " << set_duration.count() << " ms" << std::endl;
    std::cout << "Ratio (AVL/set): " << (double)avl_duration.count() / set_duration.count() << std::endl;
    std::cout << "AVL Height: " << avl_tree->getHeight() << std::endl;
    
    EXPECT_EQ(avl_tree->size(), std_set->size());
}

TEST_F(AVLTreeBenchmark, RandomInsertionBenchmark) {
    generateTestData();
    
    // AVL Tree insertion
    auto avl_start = std::chrono::high_resolution_clock::now();
    for (int value : random_data) {
        avl_tree->insert(value);
    }
    auto avl_end = std::chrono::high_resolution_clock::now();
    auto avl_duration = std::chrono::duration_cast<std::chrono::milliseconds>(avl_end - avl_start);
    
    // std::set insertion
    auto set_start = std::chrono::high_resolution_clock::now();
    for (int value : random_data) {
        std_set->insert(value);
    }
    auto set_end = std::chrono::high_resolution_clock::now();
    auto set_duration = std::chrono::duration_cast<std::chrono::milliseconds>(set_end - set_start);
    
    std::cout << "\n=== Random Insertion Benchmark ===" << std::endl;
    std::cout << "AVL Tree: " << avl_duration.count() << " ms" << std::endl;
    std::cout << "std::set: " << set_duration.count() << " ms" << std::endl;
    std::cout << "Ratio (AVL/set): " << (double)avl_duration.count() / set_duration.count() << std::endl;
    std::cout << "AVL Height: " << avl_tree->getHeight() << std::endl;
    
    EXPECT_EQ(avl_tree->size(), std_set->size());
}

TEST_F(AVLTreeBenchmark, LookupBenchmark) {
    generateTestData();
    
    // Build both trees with the same data
    for (int value : unique_random_data) {
        avl_tree->insert(value);
        std_set->insert(value);
    }
    
    // Test lookup performance
    auto avl_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_ELEMENTS / 10; ++i) {
        EXPECT_TRUE(avl_tree->contains(unique_random_data[i]));
    }
    auto avl_end = std::chrono::high_resolution_clock::now();
    auto avl_duration = std::chrono::duration_cast<std::chrono::microseconds>(avl_end - avl_start);
    
    auto set_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_ELEMENTS / 10; ++i) {
        EXPECT_TRUE(std_set->find(unique_random_data[i]) != std_set->end());
    }
    auto set_end = std::chrono::high_resolution_clock::now();
    auto set_duration = std::chrono::duration_cast<std::chrono::microseconds>(set_end - set_start);
    
    std::cout << "\n=== Lookup Benchmark ===" << std::endl;
    std::cout << "AVL Tree: " << avl_duration.count() << " μs" << std::endl;
    std::cout << "std::set: " << set_duration.count() << " μs" << std::endl;
    std::cout << "Ratio (AVL/set): " << (double)avl_duration.count() / set_duration.count() << std::endl;
}

TEST_F(AVLTreeBenchmark, DeletionBenchmark) {
    generateTestData();
    
    // Build both trees with the same data
    std::vector<int> deletion_data = unique_random_data;
    for (int value : deletion_data) {
        avl_tree->insert(value);
        std_set->insert(value);
    }
    
    // Shuffle deletion order
    std::random_device rd;
    std::shuffle(deletion_data.begin(), deletion_data.end(), std::mt19937(rd()));
    
    // AVL Tree deletion
    auto avl_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_ELEMENTS / 2; ++i) {
        avl_tree->remove(deletion_data[i]);
    }
    auto avl_end = std::chrono::high_resolution_clock::now();
    auto avl_duration = std::chrono::duration_cast<std::chrono::milliseconds>(avl_end - avl_start);
    
    // std::set deletion
    auto set_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_ELEMENTS / 2; ++i) {
        std_set->erase(deletion_data[i]);
    }
    auto set_end = std::chrono::high_resolution_clock::now();
    auto set_duration = std::chrono::duration_cast<std::chrono::milliseconds>(set_end - set_start);
    
    std::cout << "\n=== Deletion Benchmark ===" << std::endl;
    std::cout << "AVL Tree: " << avl_duration.count() << " ms" << std::endl;
    std::cout << "std::set: " << set_duration.count() << " ms" << std::endl;
    std::cout << "Ratio (AVL/set): " << (double)avl_duration.count() / set_duration.count() << std::endl;
    std::cout << "Remaining AVL size: " << avl_tree->size() << std::endl;
    std::cout << "Remaining set size: " << std_set->size() << std::endl;
    
    EXPECT_EQ(avl_tree->size(), std_set->size());
}

TEST_F(AVLTreeBenchmark, SpecialOperationsBenchmark) {
    generateTestData();
    
    // Build both trees
    for (int value : unique_random_data) {
        avl_tree->insert(value);
        std_set->insert(value);
    }
    
    // Test countSmallerThan equivalent for std::set
    auto count_smaller_avl_start = std::chrono::high_resolution_clock::now();
    [[maybe_unused]] size_t avl_count = avl_tree->countSmallerThan(NUM_ELEMENTS / 2);
    auto count_smaller_avl_end = std::chrono::high_resolution_clock::now();
    auto count_smaller_avl_duration = std::chrono::duration_cast<std::chrono::microseconds>(
        count_smaller_avl_end - count_smaller_avl_start);
    
    // For std::set, we need to count manually (O(n) implementation)
    auto count_smaller_set_start = std::chrono::high_resolution_clock::now();
    size_t set_count = std::distance(std_set->begin(), std_set->lower_bound(NUM_ELEMENTS / 2));
    auto count_smaller_set_end = std::chrono::high_resolution_clock::now();
    auto count_smaller_set_duration = std::chrono::duration_cast<std::chrono::microseconds>(
        count_smaller_set_end - count_smaller_set_start);
    
    std::cout << "\n=== Special Operations Benchmark ===" << std::endl;
    std::cout << "countSmallerThan:" << std::endl;
    std::cout << "  AVL Tree: " << count_smaller_avl_duration.count() << " μs" << std::endl;
    std::cout << "  std::set: " << count_smaller_set_duration.count() << " μs" << std::endl;
    std::cout << "  Ratio (AVL/set): " << (double)count_smaller_avl_duration.count() / count_smaller_set_duration.count() << std::endl;
    
    EXPECT_EQ(avl_count, set_count);
}

TEST_F(AVLTreeBenchmark, NthSmallestBenchmark) {
    generateTestData();
    
    // Build both trees
    for (int value : unique_random_data) {
        avl_tree->insert(value);
        std_set->insert(value);
    }
    
    // Test findNthSmallest
    auto nth_avl_start = std::chrono::high_resolution_clock::now();
    auto avl_result = avl_tree->findNthSmallest(NUM_ELEMENTS / 2);
    auto nth_avl_end = std::chrono::high_resolution_clock::now();
    auto nth_avl_duration = std::chrono::duration_cast<std::chrono::microseconds>(
        nth_avl_end - nth_avl_start);
    
    // For std::set, we need to iterate to find nth smallest
    auto nth_set_start = std::chrono::high_resolution_clock::now();
    auto set_it = std_set->begin();
    std::advance(set_it, NUM_ELEMENTS / 2 - 1);
    int set_result = *set_it;
    auto nth_set_end = std::chrono::high_resolution_clock::now();
    auto nth_set_duration = std::chrono::duration_cast<std::chrono::microseconds>(
        nth_set_end - nth_set_start);
    
    std::cout << "\n=== Nth Smallest Benchmark ===" << std::endl;
    std::cout << "findNthSmallest:" << std::endl;
    std::cout << "  AVL Tree: " << nth_avl_duration.count() << " μs" << std::endl;
    std::cout << "  std::set: " << nth_set_duration.count() << " μs" << std::endl;
    std::cout << "  Ratio (AVL/set): " << (double)nth_avl_duration.count() / nth_set_duration.count() << std::endl;
    
    EXPECT_TRUE(avl_result.has_value());
    EXPECT_EQ(avl_result.value(), set_result);
}

TEST_F(AVLTreeBenchmark, MemoryUsageComparison) {
    generateTestData();
    
    // Build both trees
    for (int value : unique_random_data) {
        avl_tree->insert(value);
        std_set->insert(value);
    }
    
    std::cout << "\n=== Memory Usage Comparison ===" << std::endl;
    std::cout << "Both trees contain: " << NUM_ELEMENTS << " elements" << std::endl;
    std::cout << "AVL Tree height: " << avl_tree->getHeight() << std::endl;
    std::cout << "Note: Actual memory usage would require specialized tools" << std::endl;
    std::cout << "std::set is typically implemented as Red-Black Tree" << std::endl;
}

TEST_F(AVLTreeBenchmark, MixedOperationsBenchmark) {
    const int OPERATIONS = 50000;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> op_dis(0, 2); // 0:insert, 1:remove, 2:contains
    std::uniform_int_distribution<> val_dis(0, OPERATIONS * 2);
    
    // AVL Tree mixed operations
    auto avl_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < OPERATIONS; ++i) {
        int operation = op_dis(gen);
        int value = val_dis(gen);
        
        switch (operation) {
            case 0: avl_tree->insert(value); break;
            case 1: avl_tree->remove(value); break;
            case 2: avl_tree->contains(value); break;
        }
    }
    auto avl_end = std::chrono::high_resolution_clock::now();
    auto avl_duration = std::chrono::duration_cast<std::chrono::milliseconds>(avl_end - avl_start);
    
    // std::set mixed operations
    auto set_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < OPERATIONS; ++i) {
        int operation = op_dis(gen);
        int value = val_dis(gen);
        
        switch (operation) {
            case 0: std_set->insert(value); break;
            case 1: std_set->erase(value); break;
            case 2: std_set->find(value); break;
        }
    }
    auto set_end = std::chrono::high_resolution_clock::now();
    auto set_duration = std::chrono::duration_cast<std::chrono::milliseconds>(set_end - set_start);
    
    std::cout << "\n=== Mixed Operations Benchmark ===" << std::endl;
    std::cout << "Operations: " << OPERATIONS << std::endl;
    std::cout << "AVL Tree: " << avl_duration.count() << " ms" << std::endl;
    std::cout << "std::set: " << set_duration.count() << " ms" << std::endl;
    std::cout << "Ratio (AVL/set): " << (double)avl_duration.count() / set_duration.count() << std::endl;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}