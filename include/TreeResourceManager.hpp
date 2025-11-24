#pragma once
#include <memory>
#include <vector>
#include <optional>
#include <queue>
#include "TreeNode.hpp"
#include "TreeResourceHandle.hpp"

class TreeResourceManager {
public:
    enum class CreateResult {
        Success,
        TooManyNodes,
        InvalidValue
    };

     // Use the public handle type
    using NodeHandle = TreeResourceHandle;

private:
    struct NodeEntry {
        std::unique_ptr<TreeNode> node;
        uint32_t generation = 1;
        bool isActive = false;
    };
    
    std::vector<NodeEntry> nodes;
    std::queue<size_t> freeIndices;
    size_t maxNodes = 1000000;
    
    bool isValidValue([[maybe_unused]] int value) const {
        return true;
    }

public:
    explicit TreeResourceManager(size_t maxNodes = 1000000) 
        : maxNodes(maxNodes) 
    {
        nodes.reserve(std::min(maxNodes, size_t(1000)));
    }

    ~TreeResourceManager() {
        clear();
    }

    std::pair<NodeHandle, CreateResult> createNode(int value = 0) noexcept {
        if (getActiveNodeCount() >= maxNodes) {
            return {NodeHandle{}, CreateResult::TooManyNodes};
        }
        if (!isValidValue(value)) {
            return {NodeHandle{}, CreateResult::InvalidValue};
        }
        
        size_t index;
        NodeEntry* entry = nullptr;
        
        if (!freeIndices.empty()) {
            // Reuse existing slot
            index = freeIndices.front();
            freeIndices.pop();
            entry = &nodes[index];
            entry->generation++;  // CRITICAL: Different generation for reuse
        } else {
            // Create new slot
            index = nodes.size();
            nodes.push_back(NodeEntry{});
            entry = &nodes[index];
            entry->generation = 1;  // Start new slots at generation 1
        }
        
        entry->node = std::make_unique<TreeNode>();
        entry->node->value = value;
        entry->isActive = true;
        
        NodeHandle handle{index, entry->generation};
        return {handle, CreateResult::Success};
    }

    // Simple version
    NodeHandle tryCreateNode(int value = 0) noexcept {
        auto [handle, result] = createNode(value);
        return handle;
    }
    
    // Safe node access
    TreeNode* getNode(NodeHandle handle) const noexcept {
        if (!handle.isValid() || handle.index >= nodes.size()) {
            return nullptr;
        }
        
        const auto& entry = nodes[handle.index];
        if (!entry.isActive || entry.generation != handle.generation) {
            return nullptr;
        }
        
        return entry.node.get();
    }
    
    // Safe deletion
    bool deleteNode(NodeHandle handle) noexcept {
        if (!handle.isValid() || handle.index >= nodes.size()) {
            return false;
        }
        
        auto& entry = nodes[handle.index];
        if (!entry.isActive || entry.generation != handle.generation) {
            return false;
        }
        
        // Reset the node and mark as free
        entry.node.reset();
        entry.isActive = false;
        freeIndices.push(handle.index);
        
        return true;
    }
    
    // Safe node validation
    bool isValidHandle(NodeHandle handle) const noexcept {
        return getNode(handle) != nullptr;
    }
    
    void clear() noexcept {
        nodes.clear();
        while (!freeIndices.empty()) freeIndices.pop();
    }
    
    size_t capacity() const noexcept { 
        return maxNodes; 
    }
    
    size_t getTotalNodeCount() const noexcept { 
        return nodes.size(); 
    }
    
    size_t getActiveNodeCount() const noexcept {
        return nodes.size() - freeIndices.size();
    }
    
    size_t getAvailableCapacity() const noexcept {
        return maxNodes - getActiveNodeCount();
    }

    // Prevent copying
    TreeResourceManager(const TreeResourceManager&) = delete;
    TreeResourceManager& operator=(const TreeResourceManager&) = delete;
    
    // Allow moving
    TreeResourceManager(TreeResourceManager&&) = default;
    TreeResourceManager& operator=(TreeResourceManager&&) = default;
};