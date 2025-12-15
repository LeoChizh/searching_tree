#include <gtest/gtest.h>
#include "AVLTree.hpp"

class AVLTreeCountSmallerThanTest : public ::testing::Test {
protected:
    AVLTree tree;
};

TEST_F(AVLTreeCountSmallerThanTest, EmptyTree) {
    EXPECT_EQ(tree.countSmallerThan(10), 0);
    EXPECT_EQ(tree.countSmallerThan(0), 0);
    EXPECT_EQ(tree.countSmallerThan(-10), 0);
}

TEST_F(AVLTreeCountSmallerThanTest, SingleElement) {
    tree.insert(5);
    
    EXPECT_EQ(tree.countSmallerThan(5), 0);   // No values < 5
    EXPECT_EQ(tree.countSmallerThan(4), 0);   // No values < 4
    EXPECT_EQ(tree.countSmallerThan(6), 1);   // 5 < 6
    EXPECT_EQ(tree.countSmallerThan(10), 1);  // 5 < 10
}

TEST_F(AVLTreeCountSmallerThanTest, TwoElements) {
    tree.insert(20);
    tree.insert(10);
    
    // Tree: 10, 20
    EXPECT_EQ(tree.countSmallerThan(10), 0);   // No values < 10
    EXPECT_EQ(tree.countSmallerThan(15), 1);   // 10 < 15
    EXPECT_EQ(tree.countSmallerThan(20), 1);   // 10 < 20
    EXPECT_EQ(tree.countSmallerThan(25), 2);   // 10,20 < 25
}

TEST_F(AVLTreeCountSmallerThanTest, ThreeElements) {
    tree.insert(50);
    tree.insert(30);
    tree.insert(70);
    
    // Sorted: 30, 50, 70
    EXPECT_EQ(tree.countSmallerThan(30), 0);   // No values < 30
    EXPECT_EQ(tree.countSmallerThan(40), 1);   // 30 < 40
    EXPECT_EQ(tree.countSmallerThan(50), 1);   // 30 < 50
    EXPECT_EQ(tree.countSmallerThan(60), 2);   // 30,50 < 60
    EXPECT_EQ(tree.countSmallerThan(70), 2);   // 30,50 < 70
    EXPECT_EQ(tree.countSmallerThan(100), 3);  // 30,50,70 < 100
}

TEST_F(AVLTreeCountSmallerThanTest, MultipleElements) {
    tree.insert(50);
    tree.insert(30);
    tree.insert(70);
    tree.insert(20);
    tree.insert(40);
    tree.insert(60);
    tree.insert(80);
    
    // Sorted: 20, 30, 40, 50, 60, 70, 80
    EXPECT_EQ(tree.countSmallerThan(20), 0);   // No values < 20
    EXPECT_EQ(tree.countSmallerThan(25), 1);   // 20 < 25
    EXPECT_EQ(tree.countSmallerThan(30), 1);   // 20 < 30
    EXPECT_EQ(tree.countSmallerThan(35), 2);   // 20,30 < 35
    EXPECT_EQ(tree.countSmallerThan(45), 3);   // 20,30,40 < 45
    EXPECT_EQ(tree.countSmallerThan(50), 3);   // 20,30,40 < 50
    EXPECT_EQ(tree.countSmallerThan(65), 5);   // 20,30,40,50,60 < 65
    EXPECT_EQ(tree.countSmallerThan(100), 7);  // All 7 values < 100
}

TEST_F(AVLTreeCountSmallerThanTest, NegativeValues) {
    tree.insert(-10);
    tree.insert(-20);
    tree.insert(-5);
    tree.insert(0);
    tree.insert(10);
    
    // Sorted: -20, -10, -5, 0, 10
    EXPECT_EQ(tree.countSmallerThan(-20), 0);   // No values < -20
    EXPECT_EQ(tree.countSmallerThan(-15), 1);   // -20 < -15
    EXPECT_EQ(tree.countSmallerThan(-10), 1);   // -20 < -10
    EXPECT_EQ(tree.countSmallerThan(-5), 2);    // -20,-10 < -5
    EXPECT_EQ(tree.countSmallerThan(0), 3);     // -20,-10,-5 < 0
    EXPECT_EQ(tree.countSmallerThan(5), 4);     // -20,-10,-5,0 < 5
    EXPECT_EQ(tree.countSmallerThan(15), 5);    // All 5 values < 15
}

TEST_F(AVLTreeCountSmallerThanTest, ExactMatches) {
    tree.insert(10);
    tree.insert(20);
    tree.insert(30);
    tree.insert(40);
    
    // Test exact matches with values in tree
    EXPECT_EQ(tree.countSmallerThan(10), 0);   // No values < 10
    EXPECT_EQ(tree.countSmallerThan(20), 1);   // 10 < 20
    EXPECT_EQ(tree.countSmallerThan(30), 2);   // 10,20 < 30
    EXPECT_EQ(tree.countSmallerThan(40), 3);   // 10,20,30 < 40
    EXPECT_EQ(tree.countSmallerThan(50), 4);   // All 4 values < 50
}

TEST_F(AVLTreeCountSmallerThanTest, LargeGap) {
    tree.insert(100);
    tree.insert(500);
    tree.insert(1000);
    
    // Sorted: 100, 500, 1000
    EXPECT_EQ(tree.countSmallerThan(100), 0);     // No values < 100
    EXPECT_EQ(tree.countSmallerThan(200), 1);     // 100 < 200
    EXPECT_EQ(tree.countSmallerThan(600), 2);     // 100,500 < 600
    EXPECT_EQ(tree.countSmallerThan(2000), 3);    // All 3 values < 2000
}

TEST_F(AVLTreeCountSmallerThanTest, AfterRebalancing) {
    // Insert to cause rebalancing
    tree.insert(30);
    tree.insert(20);
    tree.insert(10); // Should trigger right rotation
    
    // After rebalancing: 20, 10, 30
    EXPECT_EQ(tree.countSmallerThan(10), 0);   // No values < 10
    EXPECT_EQ(tree.countSmallerThan(15), 1);   // 10 < 15
    EXPECT_EQ(tree.countSmallerThan(20), 1);   // 10 < 20
    EXPECT_EQ(tree.countSmallerThan(25), 2);   // 10,20 < 25
    EXPECT_EQ(tree.countSmallerThan(35), 3);   // All 3 values < 35
}

TEST_F(AVLTreeCountSmallerThanTest, AfterRemovals) {
    tree.insert(50);
    tree.insert(30);
    tree.insert(70);
    tree.insert(20);
    tree.insert(40);
    
    // Initial: 20, 30, 40, 50, 70
    EXPECT_EQ(tree.countSmallerThan(25), 1);   // 20 < 25
    
    tree.remove(20); // Remove smallest
    // After: 30, 40, 50, 70
    EXPECT_EQ(tree.countSmallerThan(30), 0);   // No values < 30
    EXPECT_EQ(tree.countSmallerThan(35), 1);   // 30 < 35
    
    tree.remove(30); // Remove new smallest
    // After: 40, 50, 70
    EXPECT_EQ(tree.countSmallerThan(40), 0);   // No values < 40
    EXPECT_EQ(tree.countSmallerThan(45), 1);   // 40 < 45
    EXPECT_EQ(tree.countSmallerThan(60), 2);   // 40,50 < 60
}

TEST_F(AVLTreeCountSmallerThanTest, DuplicatePrevention) {
    tree.insert(50);
    tree.insert(30);
    tree.insert(70);
    tree.insert(30); // Duplicate - should be ignored
    tree.insert(50); // Duplicate - should be ignored
    
    // Sorted: 30, 50, 70
    EXPECT_EQ(tree.countSmallerThan(30), 0);   // No values < 30
    EXPECT_EQ(tree.countSmallerThan(40), 1);   // 30 < 40
    EXPECT_EQ(tree.countSmallerThan(50), 1);   // 30 < 50
    EXPECT_EQ(tree.countSmallerThan(60), 2);   // 30,50 < 60
    EXPECT_EQ(tree.countSmallerThan(100), 3);  // All 3 values < 100
}

TEST_F(AVLTreeCountSmallerThanTest, ComplexTreeStructure) {
    // Build complex balanced tree
    tree.insert(50);
    tree.insert(25);
    tree.insert(75);
    tree.insert(10);
    tree.insert(30);
    tree.insert(60);
    tree.insert(80);
    tree.insert(5);
    tree.insert(15);
    tree.insert(27);
    tree.insert(55);
    
    // Sorted: 5, 10, 15, 25, 27, 30, 50, 55, 60, 75, 80
    EXPECT_EQ(tree.countSmallerThan(5), 0);    // No values < 5
    EXPECT_EQ(tree.countSmallerThan(6), 1);    // 5 < 6
    EXPECT_EQ(tree.countSmallerThan(10), 1);   // 5 < 10
    EXPECT_EQ(tree.countSmallerThan(20), 3);   // 5,10,15 < 20
    EXPECT_EQ(tree.countSmallerThan(50), 6);   // 5,10,15,25,27,30 < 50
    EXPECT_EQ(tree.countSmallerThan(100), 11); // All 11 values < 100
}

TEST_F(AVLTreeCountSmallerThanTest, BoundaryCases) {
    tree.insert(0);
    tree.insert(100);
    
    EXPECT_EQ(tree.countSmallerThan(0), 0);    // No values < 0
    EXPECT_EQ(tree.countSmallerThan(1), 1);    // 0 < 1
    EXPECT_EQ(tree.countSmallerThan(50), 1);   // 0 < 50
    EXPECT_EQ(tree.countSmallerThan(100), 1);  // 0 < 100
    EXPECT_EQ(tree.countSmallerThan(150), 2);  // 0,100 < 150
}

TEST_F(AVLTreeCountSmallerThanTest, SequentialCalls) {
    tree.insert(50);
    tree.insert(30);
    tree.insert(70);
    tree.insert(20);
    tree.insert(40);
    
    // Multiple sequential calls should work correctly
    EXPECT_EQ(tree.countSmallerThan(20), 0);
    EXPECT_EQ(tree.countSmallerThan(25), 1);
    EXPECT_EQ(tree.countSmallerThan(35), 2);
    EXPECT_EQ(tree.countSmallerThan(45), 3);
    EXPECT_EQ(tree.countSmallerThan(60), 4);
    
    // Call again - should return same results
    EXPECT_EQ(tree.countSmallerThan(20), 0);
    EXPECT_EQ(tree.countSmallerThan(60), 4);
}

TEST_F(AVLTreeCountSmallerThanTest, AllEqual) {
    tree.insert(5);
    tree.insert(5); // Duplicate - ignored
    tree.insert(5); // Duplicate - ignored
    
    EXPECT_EQ(tree.countSmallerThan(5), 0);   // No values < 5
    EXPECT_EQ(tree.countSmallerThan(6), 1);   // 5 < 6
    EXPECT_EQ(tree.countSmallerThan(10), 1);  // 5 < 10
}