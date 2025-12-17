#pragma once
#include "TreeNodePool.hpp"
#include "TreeStructure.hpp"
#include <queue>
#include <utility>
#include <optional>
#include <string>

class AVLTree {
public:
    using NodeHandle = TreeResourceHandle;
    
    // Constructors & Destructor
    explicit AVLTree();
    ~AVLTree();
    AVLTree(const AVLTree& other);
    AVLTree(AVLTree&& other) noexcept;
    AVLTree& operator=(const AVLTree& other);
    AVLTree& operator=(AVLTree&& other) noexcept;
    
    // Public interface
    bool empty() const;
    bool insert(int value);
    bool remove(int value);
    bool contains(int value) const;
    size_t size() const;
    int getHeight() const;
    std::string visualize() const;
    std::optional<int> findNthSmallest(size_t n) const;
    size_t countSmallerThan(int target) const;
    void swap(AVLTree& other) noexcept;

private:
    TreeNodePool pool;
    NodeHandle root;
    
    // Private methods
    void copyAllValues(const AVLTree& other);
    void clearSubtreeIterative(NodeHandle nodeHandle);
    std::pair<NodeHandle, bool> insertNode(NodeHandle nodeHandle, int value);
    std::pair<NodeHandle, bool> removeNode(NodeHandle nodeHandle, int value);
    NodeHandle findNode(NodeHandle nodeHandle, int value) const;
    NodeHandle findMin(NodeHandle nodeHandle) const;
    void updateNode(NodeHandle nodeHandle);
    NodeHandle balance(NodeHandle nodeHandle);
    NodeHandle rotateLeft(NodeHandle nodeHandle);
    NodeHandle rotateRight(NodeHandle nodeHandle);
    int calculateHeight(NodeHandle nodeHandle) const;
    size_t countNodes(NodeHandle nodeHandle) const;
    std::string visualizeSubtree(NodeHandle node, int depth) const;
    std::optional<int> findNthSmallestSubtree(NodeHandle node, size_t n) const;
    size_t countSmallerThanSubtree(NodeHandle node, int target) const;

    // Prevent dangerous operations
    AVLTree(const AVLTree&&) = delete;
    AVLTree& operator=(const AVLTree&&) = delete;
};