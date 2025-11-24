#pragma once

// Simple data container with minimal, const helpers
struct TreeNode {
    TreeNode* parent = nullptr;
    TreeNode* left = nullptr;
    TreeNode* right = nullptr;
    int value = 0;
    int balancer = 0;
    
    TreeNode() = default;
    TreeNode(TreeNode* p, TreeNode* l, TreeNode* r, int val) 
        : parent(p), left(l), right(r), value(val) {}
    
    // Only simple const queries about current state
    bool isLeaf() const { return !left && !right; }
    bool isRoot() const { return !parent; }
    bool hasLeft() const { return left != nullptr; }
    bool hasRight() const { return right != nullptr; }
    
};
