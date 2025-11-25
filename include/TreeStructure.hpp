#pragma once
#include <optional>
#include "TreeNodePool.hpp"
#include "TreeResourceHandle.hpp"


// TreeStructure as a namespace or static class with pure functions
namespace TreeStructure {
    using NodeHandle = TreeResourceHandle;
    // Tree structure operations
    inline bool setLeftChild(TreeNodePool& pool, NodeHandle parent, NodeHandle leftChild) noexcept {
        TreeNode* parentNode = pool.accessNode(parent);
        if (!parentNode) return false;
        
        // Clear old child's parent reference if it exists
        NodeHandle oldLeftChild = parentNode->left;
        if (oldLeftChild.isValid()) {
            TreeNode* oldLeftNode = pool.accessNode(oldLeftChild);
            if (oldLeftNode && oldLeftNode->parent == parent) {
                oldLeftNode->parent = NodeHandle{}; // Clear parent reference
            }
        }
        
        // Set new left child
        parentNode->left = leftChild;
        
        // Update new child's parent reference if child exists and is valid
        if (leftChild.isValid()) {
            TreeNode* childNode = pool.accessNode(leftChild);
            if (childNode) {
                childNode->parent = parent;
            }
        }
        return true;
    }

    inline bool setRightChild(TreeNodePool& pool, NodeHandle parent, NodeHandle rightChild) noexcept {
        TreeNode* parentNode = pool.accessNode(parent);
        if (!parentNode) return false;
        
        // Clear old child's parent reference if it exists
        NodeHandle oldRightChild = parentNode->right;
        if (oldRightChild.isValid()) {
            TreeNode* oldRightNode = pool.accessNode(oldRightChild);
            if (oldRightNode && oldRightNode->parent == parent) {
                oldRightNode->parent = NodeHandle{}; // Clear parent reference
            }
        }
        
        // Set new right child
        parentNode->right = rightChild;
        
        // Update new child's parent reference if child exists and is valid
        if (rightChild.isValid()) {
            TreeNode* childNode = pool.accessNode(rightChild);
            if (childNode) {
                childNode->parent = parent;
            }
        }
        return true;
    }
    
    inline NodeHandle getLeftChild(const TreeNodePool& pool, NodeHandle node) noexcept {
        const TreeNode* nodePtr = pool.accessNode(node);
        return nodePtr ? nodePtr->left : NodeHandle{};
    }
    
    inline NodeHandle getRightChild(const TreeNodePool& pool, NodeHandle node) noexcept {
        const TreeNode* nodePtr = pool.accessNode(node);
        return nodePtr ? nodePtr->right : NodeHandle{};
    }
    
    inline NodeHandle getParent(const TreeNodePool& pool, NodeHandle node) noexcept {
        const TreeNode* nodePtr = pool.accessNode(node);
        return nodePtr ? nodePtr->parent : NodeHandle{};
    }
    
    // Node data operations
    inline bool setNodeValue(TreeNodePool& pool, NodeHandle node, int value) noexcept {
        TreeNode* nodePtr = pool.accessNode(node);
        if (!nodePtr) return false;
        nodePtr->value = value;
        return true;
    }
    
    inline std::optional<int> getNodeValue(const TreeNodePool& pool, NodeHandle node) noexcept {
        const TreeNode* nodePtr = pool.accessNode(node);
        return nodePtr ? std::optional<int>{nodePtr->value} : std::nullopt;
    }
    
    inline bool setBalanceFactor(TreeNodePool& pool, NodeHandle node, int balance) noexcept {
        TreeNode* nodePtr = pool.accessNode(node);
        if (!nodePtr) return false;
        nodePtr->balanceFactor = balance;
        return true;
    }
    
    inline std::optional<int> getBalanceFactor(const TreeNodePool& pool, NodeHandle node) noexcept {
        const TreeNode* nodePtr = pool.accessNode(node);
        return nodePtr ? std::optional<int>{nodePtr->balanceFactor} : std::nullopt;
    }
    
    inline bool setHeight(TreeNodePool& pool, NodeHandle node, int height) noexcept {
        TreeNode* nodePtr = pool.accessNode(node);
        if (!nodePtr) return false;
        nodePtr->height = height;
        return true;
    }
    
    inline std::optional<int> getHeight(const TreeNodePool& pool, NodeHandle node) noexcept {
        const TreeNode* nodePtr = pool.accessNode(node);
        return nodePtr ? std::optional<int>{nodePtr->height} : std::nullopt;
    }

        // Subtree size operations - ADD THESE
    inline bool setSubtreeSize(TreeNodePool& pool, NodeHandle node, size_t size) noexcept {
        TreeNode* nodePtr = pool.accessNode(node);
        if (!nodePtr) return false;
        nodePtr->subtree_size = size;
        return true;
    }
    
    inline std::optional<size_t> getSubtreeSize(const TreeNodePool& pool, NodeHandle node) noexcept {
        const TreeNode* nodePtr = pool.accessNode(node);
        return nodePtr ? std::optional<size_t>{nodePtr->subtree_size} : std::nullopt;
    }
    
    // Helper function to update subtree size based on children
    inline bool updateSubtreeSize(TreeNodePool& pool, NodeHandle node) noexcept {
        if (!node.isValid()) return false;
        
        TreeNode* nodePtr = pool.accessNode(node);
        if (!nodePtr) return false;
        
        size_t left_size = 0;
        if (nodePtr->left.isValid()) {
            auto left_size_opt = getSubtreeSize(pool, nodePtr->left);
            left_size = left_size_opt.value_or(0);
        }
        
        size_t right_size = 0;
        if (nodePtr->right.isValid()) {
            auto right_size_opt = getSubtreeSize(pool, nodePtr->right);
            right_size = right_size_opt.value_or(0);
        }
        
        nodePtr->subtree_size = 1 + left_size + right_size;
        return true;
    }
    
    // Helper function to update subtree sizes along a path to root
    inline void updateSubtreeSizesToRoot(TreeNodePool& pool, NodeHandle startNode) noexcept {
        NodeHandle current = startNode;
        while (current.isValid()) {
            updateSubtreeSize(pool, current);
            current = getParent(pool, current);
        }
    }
}
