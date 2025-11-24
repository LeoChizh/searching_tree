#pragma once
#include "TreeNode.hpp"
#include "TreeResourceManager.hpp"
#include <stdexcept>

// Single Responsibility: ONLY manages tree structure and algorithms
class BinaryTree {
private:
    TreeResourceManager& resourceManager;
    TreeNode* root = nullptr;

public:
    explicit BinaryTree(TreeResourceManager& rm) : resourceManager(rm) {}
    
    // Tree operations
    TreeNode* insert(int value) {
        TreeNode* newNode = resourceManager.createNode(value);
        
        if (!root) {
            root = newNode;
            return newNode;
        }
        
        return insertNode(root, newNode);
    }
    
    bool remove(int value) {
        TreeNode* node = find(value);
        if (!node) return false;
        
        removeNode(node);
        return true;
    }
    
    TreeNode* find(int value) const {
        return findNode(root, value);
    }
    
    // Relationship management
    bool setLeft(TreeNode* parent, TreeNode* child) {
        if (!parent || !child) return false;
        if (parent == child) return false;
        if (wouldCreateCycle(parent, child)) return false;
        
        // Update relationships
        if (parent->left && parent->left->parent == parent) {
            parent->left->parent = nullptr;
        }
        
        parent->left = child;
        
        if (child->parent && child->parent != parent) {
            // Detach from old parent
            if (child->parent->left == child) {
                child->parent->left = nullptr;
            } else if (child->parent->right == child) {
                child->parent->right = nullptr;
            }
        }
        
        child->parent = parent;
        return true;
    }
    
    bool setRight(TreeNode* parent, TreeNode* child) {
        // Similar implementation to setLeft
        if (!parent || !child) return false;
        if (parent == child) return false;
        if (wouldCreateCycle(parent, child)) return false;
        
        if (parent->right && parent->right->parent == parent) {
            parent->right->parent = nullptr;
        }
        
        parent->right = child;
        
        if (child->parent && child->parent != parent) {
            if (child->parent->left == child) {
                child->parent->left = nullptr;
            } else if (child->parent->right == child) {
                child->parent->right = nullptr;
            }
        }
        
        child->parent = parent;
        return true;
    }
    
    TreeNode* getRoot() const { return root; }

private:
    TreeNode* insertNode(TreeNode* current, TreeNode* newNode) {
        if (newNode->value < current->value) {
            if (!current->left) {
                current->left = newNode;
                newNode->parent = current;
                return newNode;
            }
            return insertNode(current->left, newNode);
        } else {
            if (!current->right) {
                current->right = newNode;
                newNode->parent = current;
                return newNode;
            }
            return insertNode(current->right, newNode);
        }
    }
    
    TreeNode* findNode(TreeNode* current, int value) const {
        if (!current) return nullptr;
        if (value == current->value) return current;
        
        if (value < current->value) {
            return findNode(current->left, value);
        } else {
            return findNode(current->right, value);
        }
    }
    
    void removeNode(TreeNode* node) {
        // Implementation of node removal
        // Would call resourceManager.deleteNode(node) when appropriate
    }
    
    bool wouldCreateCycle(TreeNode* parent, TreeNode* child) const {
        TreeNode* current = parent;
        while (current) {
            if (current == child) return true;
            current = current->parent;
        }
        return false;
    }
};