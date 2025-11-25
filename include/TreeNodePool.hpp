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

    explicit TreeNodePool(size_t maxNodes = 1000000) 
        : maxNodes(maxNodes) 
    {
        nodes.reserve(std::min(maxNodes, size_t(1000)));
    }

    ~TreeNodePool() {
        clear();
    }

    // Core memory management
    std::pair<NodeHandle, CreateResult> createNode() noexcept {
        if (getActiveNodeCount() >= maxNodes) {
            return {NodeHandle{}, CreateResult::TooManyNodes};
        }
        
        size_t index;
        NodeEntry* entry = nullptr;
        
        if (!freeIndices.empty()) {
            index = freeIndices.front();
            freeIndices.pop();
            entry = &nodes[index];
            entry->generation++;
        } else {
            index = nodes.size();
            nodes.push_back(NodeEntry{});
            entry = &nodes[index];
            entry->generation = 1;
        }
        
        entry->node = TreeNode{};
        entry->isActive = true;
        
        NodeHandle handle{index, entry->generation};
        return {handle, CreateResult::Success};
    }

    NodeHandle tryCreateNode() noexcept {
        auto [handle, result] = createNode();
        return handle;
    }
    
    // Fast, unsafe access - caller handles validation
    TreeNode* accessNode(NodeHandle handle) noexcept {
        if (!handle.isValid() || handle.index >= nodes.size()) {
            return nullptr;
        }
        
        auto& entry = nodes[handle.index];
        if (!entry.isActive || entry.generation != handle.generation) {
            return nullptr;
        }
        
        return &entry.node;
    }
    
    const TreeNode* accessNode(NodeHandle handle) const noexcept {
        return const_cast<TreeNodePool*>(this)->accessNode(handle);
    }
    
    bool deleteNode(NodeHandle handle) noexcept {
        TreeNode* node = accessNode(handle);
        if (!node) return false;
        
        *node = TreeNode{};  // Reset node data
        nodes[handle.index].isActive = false;
        freeIndices.push(handle.index);
        
        return true;
    }
    
    bool isValidHandle(NodeHandle handle) const noexcept {
        return accessNode(handle) != nullptr;
    }
    
    void clear() noexcept {
        nodes.clear();
        while (!freeIndices.empty()) freeIndices.pop();
    }
    
    // Memory management info
    size_t capacity() const noexcept { return maxNodes; }
    size_t getTotalNodeCount() const noexcept { return nodes.size(); }
    size_t getActiveNodeCount() const noexcept { return nodes.size() - freeIndices.size(); }
    size_t getAvailableCapacity() const noexcept { return maxNodes - getActiveNodeCount(); }

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