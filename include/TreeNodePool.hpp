#pragma once
#include <vector>
#include <queue>
#include "TreeNode.hpp"
#include "TreeResourceHandle.hpp"

class TreeNodePool {
public:
    enum class CreateResult {
        Success,
        TooManyNodes
    };

    using NodeHandle = TreeResourceHandle;

    explicit TreeNodePool(size_t maxNodes = 1000000);
    ~TreeNodePool() = default;

    // Core memory management
    std::pair<NodeHandle, CreateResult> createNode() noexcept;
    NodeHandle tryCreateNode() noexcept;
    TreeNode* accessNode(NodeHandle handle) noexcept;
    const TreeNode* accessNode(NodeHandle handle) const noexcept;
    bool deleteNode(NodeHandle handle) noexcept;
    bool isValidHandle(NodeHandle handle) const noexcept;
    
    // Memory management info
    size_t capacity() const noexcept;
    size_t getTotalNodeCount() const noexcept;
    size_t getActiveNodeCount() const noexcept;
    size_t getAvailableCapacity() const noexcept;

    TreeNodePool(const TreeNodePool&) = delete;
    TreeNodePool& operator=(const TreeNodePool&) = delete;
    TreeNodePool(TreeNodePool&&) = default;
    TreeNodePool& operator=(TreeNodePool&&) = default;

private:
    struct NodeEntry {
        TreeNode node;
        uint32_t generation = 1;
        bool isActive = false;
    };
    
    std::vector<NodeEntry> nodes;
    std::queue<size_t> freeIndices;
    size_t maxNodes = 1000000;
};