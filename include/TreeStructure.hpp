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
}