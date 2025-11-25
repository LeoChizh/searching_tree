#pragma once
#include "TreeResourceHandle.hpp"

// TreeNode using handles instead of raw pointers
struct TreeNode {
    TreeResourceHandle parent;
    TreeResourceHandle left;
    TreeResourceHandle right;
    int value = 0;
    int balanceFactor = 0;
    int height = 0;
    size_t subtree_size = 1;  
    
    TreeNode() = default;
    
    bool isLeaf() const { return !left.isValid() && !right.isValid(); }
    bool isRoot() const { return !parent.isValid(); }
    bool hasLeft() const { return left.isValid(); }
    bool hasRight() const { return right.isValid(); }
};