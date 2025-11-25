#pragma once
#include "TreeNodePool.hpp"
#include "TreeStructure.hpp"
#include <queue>
#include <utility>

class AVLTree {
public:
    using NodeHandle = TreeResourceHandle;
    
    // 1. Constructor
    explicit AVLTree() : pool(), root() {}
    
    // 2. Destructor
    ~AVLTree() {
        clear();
    }
    
    // 3. Copy Constructor - Create new tree and insert all values
    AVLTree(const AVLTree& other) : pool(), root() {
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
        : pool(std::move(other.pool)), 
          root(std::move(other.root)) {
        other.root = NodeHandle();
    }
    
    AVLTree& operator=(AVLTree&& other) noexcept {
        if (this != &other) {
            clear();
            pool = std::move(other.pool);
            root = std::move(other.root);
            other.root = NodeHandle();
        }
        return *this;
    }
    
    void swap(AVLTree& other) noexcept {
        using std::swap;
        swap(pool, other.pool);
        swap(root, other.root);
    }
    
    // Public interface
    void clear() {
        if (root.isValid()) {
            clearSubtree(root);
            root = NodeHandle();
        }
        pool.clear(); // Clear the entire pool
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

    std::string visualize() const {
        if (empty()) return "Empty tree";
        return visualizeSubtree(root, 0);
    }

private:
    TreeNodePool pool;
    NodeHandle root;
    
    // Level-order copy: Create new tree by inserting all values
    void copyAllValues(const AVLTree& other) {
        if (other.empty()) return;
        
        std::queue<NodeHandle> q;
        q.push(other.root);
        
        while (!q.empty()) {
            NodeHandle current = q.front();
            q.pop();
            
            // Use TreeStructure to get node value
            auto value = TreeStructure::getNodeValue(other.pool, current);
            if (!value.has_value()) continue;
            
            // Insert value into this tree
            insert(value.value());
            
            // Add children to queue using TreeStructure
            NodeHandle leftChild = TreeStructure::getLeftChild(other.pool, current);
            if (leftChild.isValid()) {
                q.push(leftChild);
            }
            
            NodeHandle rightChild = TreeStructure::getRightChild(other.pool, current);
            if (rightChild.isValid()) {
                q.push(rightChild);
            }
        }
    }
    
    // Recursive cleanup
    void clearSubtree(NodeHandle nodeHandle) {
        if (!nodeHandle.isValid()) return;
        
        // Get children first (before deletion)
        NodeHandle leftChild = TreeStructure::getLeftChild(pool, nodeHandle);
        NodeHandle rightChild = TreeStructure::getRightChild(pool, nodeHandle);
        
        // Recursively clear children
        clearSubtree(leftChild);
        clearSubtree(rightChild);
        
        // Delete this node
        pool.deleteNode(nodeHandle);
    }
    
    // AVL Tree operations - using TreeStructure
    std::pair<NodeHandle, bool> insertNode(NodeHandle nodeHandle, int value) {
        if (!nodeHandle.isValid()) {
            // Create new node
            NodeHandle newNode = pool.tryCreateNode();
            if (!newNode.isValid()) {
                return {NodeHandle(), false};
            }
            TreeStructure::setNodeValue(pool, newNode, value);
            return {newNode, true};
        }
        
        // Get current node value
        auto nodeValue = TreeStructure::getNodeValue(pool, nodeHandle);
        if (!nodeValue.has_value()) {
            return {NodeHandle(), false};
        }
        
        if (value < nodeValue.value()) {
            NodeHandle leftChild = TreeStructure::getLeftChild(pool, nodeHandle);
            auto [newLeft, inserted] = insertNode(leftChild, value);
            if (inserted) {
                TreeStructure::setLeftChild(pool, nodeHandle, newLeft);
                // Update height and balance
                updateNode(nodeHandle);
                return {balance(nodeHandle), true};
            }
        } else if (value > nodeValue.value()) {
            NodeHandle rightChild = TreeStructure::getRightChild(pool, nodeHandle);
            auto [newRight, inserted] = insertNode(rightChild, value);
            if (inserted) {
                TreeStructure::setRightChild(pool, nodeHandle, newRight);
                // Update height and balance
                updateNode(nodeHandle);
                return {balance(nodeHandle), true};
            }
        }
        
        // Value already exists
        return {nodeHandle, false};
    }
    
    std::pair<NodeHandle, bool> removeNode(NodeHandle nodeHandle, int value) {
        if (!nodeHandle.isValid()) {
            return {NodeHandle(), false};
        }
        
        auto nodeValue = TreeStructure::getNodeValue(pool, nodeHandle);
        if (!nodeValue.has_value()) {
            return {NodeHandle(), false};
        }
        
        NodeHandle leftChild = TreeStructure::getLeftChild(pool, nodeHandle);
        NodeHandle rightChild = TreeStructure::getRightChild(pool, nodeHandle);
        
        if (value < nodeValue.value()) {
            auto [newLeft, removed] = removeNode(leftChild, value);
            if (removed) {
                TreeStructure::setLeftChild(pool, nodeHandle, newLeft);
                updateNode(nodeHandle);
                return {balance(nodeHandle), true};
            }
        } else if (value > nodeValue.value()) {
            auto [newRight, removed] = removeNode(rightChild, value);
            if (removed) {
                TreeStructure::setRightChild(pool, nodeHandle, newRight);
                updateNode(nodeHandle);
                return {balance(nodeHandle), true};
            }
        } else {
            // Node to delete found
            if (!leftChild.isValid() || !rightChild.isValid()) {
                // Zero or one child
                NodeHandle child = leftChild.isValid() ? leftChild : rightChild;
                pool.deleteNode(nodeHandle);
                return {child, true};
            } else {
                // Two children: find inorder successor
                NodeHandle successor = findMin(rightChild);
                auto successorValue = TreeStructure::getNodeValue(pool, successor);
                if (!successorValue.has_value()) {
                    return {NodeHandle(), false};
                }
                
                // Copy successor's value
                TreeStructure::setNodeValue(pool, nodeHandle, successorValue.value());
                
                // Delete the successor
                auto [newRight, _] = removeNode(rightChild, successorValue.value());
                TreeStructure::setRightChild(pool, nodeHandle, newRight);
                
                updateNode(nodeHandle);
                return {balance(nodeHandle), true};
            }
        }
        
        return {nodeHandle, false};
    }
    
    NodeHandle findNode(NodeHandle nodeHandle, int value) const {
        if (!nodeHandle.isValid()) return NodeHandle();
        
        auto nodeValue = TreeStructure::getNodeValue(pool, nodeHandle);
        if (!nodeValue.has_value()) return NodeHandle();
        
        if (value < nodeValue.value()) {
            return findNode(TreeStructure::getLeftChild(pool, nodeHandle), value);
        } else if (value > nodeValue.value()) {
            return findNode(TreeStructure::getRightChild(pool, nodeHandle), value);
        } else {
            return nodeHandle; // Found
        }
    }
    
    NodeHandle findMin(NodeHandle nodeHandle) const {
        if (!nodeHandle.isValid()) return NodeHandle();
        
        NodeHandle current = nodeHandle;
        NodeHandle leftChild = TreeStructure::getLeftChild(pool, current);
        
        while (leftChild.isValid()) {
            current = leftChild;
            leftChild = TreeStructure::getLeftChild(pool, current);
        }
        
        return current;
    }
    
    // AVL Balancing operations
    void updateNode(NodeHandle nodeHandle) {
        if (!nodeHandle.isValid()) return;
        
        NodeHandle leftChild = TreeStructure::getLeftChild(pool, nodeHandle);
        NodeHandle rightChild = TreeStructure::getRightChild(pool, nodeHandle);
        
        int leftHeight = calculateHeight(leftChild);
        int rightHeight = calculateHeight(rightChild);
        
        TreeStructure::setHeight(pool, nodeHandle, 1 + std::max(leftHeight, rightHeight));
        TreeStructure::setBalanceFactor(pool, nodeHandle, leftHeight - rightHeight);
    }
    
    NodeHandle balance(NodeHandle nodeHandle) {
        auto balanceFactor = TreeStructure::getBalanceFactor(pool, nodeHandle);
        if (!balanceFactor.has_value()) return nodeHandle;
        
        // Left heavy
        if (balanceFactor.value() > 1) {
            NodeHandle leftChild = TreeStructure::getLeftChild(pool, nodeHandle);
            auto leftBalance = TreeStructure::getBalanceFactor(pool, leftChild);
            if (leftBalance.has_value() && leftBalance.value() < 0) {
                // Left-Right case
                NodeHandle newLeft = rotateLeft(leftChild);
                TreeStructure::setLeftChild(pool, nodeHandle, newLeft);
            }
            return rotateRight(nodeHandle);
        }
        // Right heavy
        else if (balanceFactor.value() < -1) {
            NodeHandle rightChild = TreeStructure::getRightChild(pool, nodeHandle);
            auto rightBalance = TreeStructure::getBalanceFactor(pool, rightChild);
            if (rightBalance.has_value() && rightBalance.value() > 0) {
                // Right-Left case
                NodeHandle newRight = rotateRight(rightChild);
                TreeStructure::setRightChild(pool, nodeHandle, newRight);
            }
            return rotateLeft(nodeHandle);
        }
        
        return nodeHandle;
    }
    
    NodeHandle rotateLeft(NodeHandle nodeHandle) {
        NodeHandle rightChild = TreeStructure::getRightChild(pool, nodeHandle);
        if (!rightChild.isValid()) return nodeHandle;
        
        NodeHandle rightLeftChild = TreeStructure::getLeftChild(pool, rightChild);
        
        TreeStructure::setLeftChild(pool, rightChild, nodeHandle);
        TreeStructure::setRightChild(pool, nodeHandle, rightLeftChild);
        
        updateNode(nodeHandle);
        updateNode(rightChild);
        
        return rightChild;
    }
    
    NodeHandle rotateRight(NodeHandle nodeHandle) {
        NodeHandle leftChild = TreeStructure::getLeftChild(pool, nodeHandle);
        if (!leftChild.isValid()) return nodeHandle;
        
        NodeHandle leftRightChild = TreeStructure::getRightChild(pool, leftChild);
        
        TreeStructure::setRightChild(pool, leftChild, nodeHandle);
        TreeStructure::setLeftChild(pool, nodeHandle, leftRightChild);
        
        updateNode(nodeHandle);
        updateNode(leftChild);
        
        return leftChild;
    }
    
    int calculateHeight(NodeHandle nodeHandle) const {
        if (!nodeHandle.isValid()) return -1;
        
        auto height = TreeStructure::getHeight(pool, nodeHandle);
        return height.has_value() ? height.value() : -1;
    }
    
    size_t countNodes(NodeHandle nodeHandle) const {
        if (!nodeHandle.isValid()) return 0;
        
        NodeHandle leftChild = TreeStructure::getLeftChild(pool, nodeHandle);
        NodeHandle rightChild = TreeStructure::getRightChild(pool, nodeHandle);
        
        return 1 + countNodes(leftChild) + countNodes(rightChild);
    }
    
    // Prevent dangerous operations
    AVLTree(const AVLTree&&) = delete;
    AVLTree& operator=(const AVLTree&&) = delete;

    std::string visualizeSubtree(NodeHandle node, int depth) const {
        if (!node.isValid()) return "";
        
        auto value = TreeStructure::getNodeValue(pool, node);
        auto balance = TreeStructure::getBalanceFactor(pool, node);
        auto height = TreeStructure::getHeight(pool, node);
        
        if (!value.has_value() || !balance.has_value() || !height.has_value()) {
            return "";
        }
        
        std::stringstream ss;
        
        // Right child first
        NodeHandle rightChild = TreeStructure::getRightChild(pool, node);
        if (rightChild.isValid()) {
            ss << visualizeSubtree(rightChild, depth + 1);
        }
        
        // Current node with indentation
        for (int i = 0; i < depth; ++i) {
            ss << "    ";
        }
        ss << value.value() << " [H:" << height.value() << " B:" << balance.value() << "]\n";
        
        // Left child last
        NodeHandle leftChild = TreeStructure::getLeftChild(pool, node);
        if (leftChild.isValid()) {
            ss << visualizeSubtree(leftChild, depth + 1);
        }
        
        return ss.str();
    }
};