#pragma once
#include "TreeNode.hpp"
#include "TreeResourceHandle.hpp"
#include "TreeResourceManager.hpp"
#include <queue>
#include <utility>

class AVLTree {
public:
    using NodeHandle = TreeResourceHandle;
    
    // 1. Constructor
    explicit AVLTree() : manager(), root() {}
    
    // 2. Destructor
    ~AVLTree() {
        clear();
    }
    
    // 3. Copy Constructor - Create new tree and insert all values
    AVLTree(const AVLTree& other) : manager(), root() {
        copyAllValues(other);
    }
    
    // 4. Copy Assignment
    AVLTree& operator=(const AVLTree& other) {
        if (this != &other) {
            clear();
            copyAllValues(other);
        }
        return *this;
    }
    
    // 5. Move Operations
    AVLTree(AVLTree&& other) noexcept 
        : manager(std::move(other.manager)), 
          root(std::move(other.root)) {
        other.root = NodeHandle();
    }
    
    AVLTree& operator=(AVLTree&& other) noexcept {
        if (this != &other) {
            clear();
            manager = std::move(other.manager);
            root = std::move(other.root);
            other.root = NodeHandle();
        }
        return *this;
    }
    
    void swap(AVLTree& other) noexcept {
        using std::swap;
        swap(manager, other.manager);
        swap(root, other.root);
    }
    
    // Public interface
    void clear() {
        if (root.isValid()) {
            clearSubtree(root);
            root = NodeHandle();
        }
    }
    
    bool empty() const { 
        return !root.isValid(); 
    }
    
    bool insert(int value) {
        auto [newRoot, inserted] = insertNode(root, value);
        if (inserted) {
            root = newRoot;
            return true;
        }
        return false;
    }
    
    bool remove(int value) {
        auto [newRoot, removed] = removeNode(root, value);
        if (removed) {
            root = newRoot;
            return true;
        }
        return false;
    }
    
    bool contains(int value) const {
        return findNode(root, value).isValid();
    }
    
    // For testing/debugging
    size_t size() const {
        return countNodes(root);
    }
    
    int getHeight() const {
        return calculateHeight(root);
    }

private:
    TreeResourceManager manager;
    NodeHandle root;
    
    // Level-order copy: Create new tree by inserting all values
    void copyAllValues(const AVLTree& other) {
        if (other.empty()) return;
        
        std::queue<NodeHandle> q;
        q.push(other.root);
        
        while (!q.empty()) {
            NodeHandle current = q.front();
            q.pop();
            
            const TreeNode* node = other.manager.getNode(current);
            if (!node) continue;
            
            // Insert value into this tree
            insert(node->value);
            
            // Add children to queue - FIX: use hasLeft()/hasRight() and get node pointers
            if (node->hasLeft()) {
                // Since TreeNode uses raw pointers, we need to find the handle for left child
                // This is the tricky part - we need to search for which handle points to node->left
                for (size_t i = 0; i < other.manager.getTotalNodeCount(); ++i) {
                    NodeHandle potentialHandle{static_cast<uint32_t>(i), 1};
                    TreeNode* potentialNode = other.manager.getNode(potentialHandle);
                    if (potentialNode == node->left) {
                        q.push(potentialHandle);
                        break;
                    }
                }
            }
            if (node->hasRight()) {
                // Same for right child
                for (size_t i = 0; i < other.manager.getTotalNodeCount(); ++i) {
                    NodeHandle potentialHandle{static_cast<uint32_t>(i), 1};
                    TreeNode* potentialNode = other.manager.getNode(potentialHandle);
                    if (potentialNode == node->right) {
                        q.push(potentialHandle);
                        break;
                    }
                }
            }
        }
    }
    
    // Recursive cleanup
    void clearSubtree(NodeHandle nodeHandle) {
        if (!nodeHandle.isValid()) return;
        
        TreeNode* node = manager.getNode(nodeHandle);
        if (!node) return;
        
        // Clear children first
        clearSubtree(node->left);
        clearSubtree(node->right);
        
        // Delete this node
        manager.deleteNode(nodeHandle);
    }
    
    // Prevent dangerous operations
    AVLTree(const AVLTree&&) = delete;
    AVLTree& operator=(const AVLTree&&) = delete;
};