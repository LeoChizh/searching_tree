#pragma once
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

    using NodeHandle = TreeResourceHandle;

private:
    struct NodeEntry {
        TreeNode node;           // Direct storage
        uint32_t generation = 1;
        bool isActive = false;
    };
    
    std::vector<NodeEntry> nodes;
    std::queue<size_t> freeIndices;
    size_t maxNodes = 1000000;

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
        entry->node.value = value;
        entry->isActive = true;
        
        NodeHandle handle{index, entry->generation};
        return {handle, CreateResult::Success};
    }

    NodeHandle tryCreateNode(int value = 0) noexcept {
        auto [handle, result] = createNode(value);
        return handle;
    }
    
    // Safe node validation
    bool isValidHandle(NodeHandle handle) const noexcept {
        if (!handle.isValid() || handle.index >= nodes.size()) {
            return false;
        }
        
        const auto& entry = nodes[handle.index];
        return entry.isActive && entry.generation == handle.generation;
    }
    
    // Node property accessors
    std::optional<int> getNodeValue(NodeHandle handle) const noexcept {
        if (!isValidHandle(handle)) return std::nullopt;
        return nodes[handle.index].node.value;
    }
    
    std::optional<int> getBalanceFactor(NodeHandle handle) const noexcept {
        if (!isValidHandle(handle)) return std::nullopt;
        return nodes[handle.index].node.balanceFactor;
    }
    
    std::optional<int> getHeight(NodeHandle handle) const noexcept {
        if (!isValidHandle(handle)) return std::nullopt;
        return nodes[handle.index].node.height;
    }
    
    // Node property setters
    bool setNodeValue(NodeHandle handle, int value) noexcept {
        if (!isValidHandle(handle)) return false;
        nodes[handle.index].node.value = value;
        return true;
    }
    
    bool setBalanceFactor(NodeHandle handle, int balance) noexcept {
        if (!isValidHandle(handle)) return false;
        nodes[handle.index].node.balanceFactor = balance;
        return true;
    }
    
    bool setHeight(NodeHandle handle, int height) noexcept {
        if (!isValidHandle(handle)) return false;
        nodes[handle.index].node.height = height;
        return true;
    }
    
    // Tree structure operations
    NodeHandle getLeftChild(NodeHandle parent) const noexcept {
        if (!isValidHandle(parent)) return NodeHandle{};
        return nodes[parent.index].node.left;
    }
    
    NodeHandle getRightChild(NodeHandle parent) const noexcept {
        if (!isValidHandle(parent)) return NodeHandle{};
        return nodes[parent.index].node.right;
    }
    
    NodeHandle getParent(NodeHandle child) const noexcept {
        if (!isValidHandle(child)) return NodeHandle{};
        return nodes[child.index].node.parent;
    }
    
    bool setLeftChild(NodeHandle parent, NodeHandle leftChild) noexcept {
        // Only validate parent - allow invalid leftChild (leaf node case)
        if (!isValidHandle(parent)) return false;
        
        nodes[parent.index].node.left = leftChild;
        
        // Update the child's parent reference only if child is valid
        if (leftChild.isValid() && isValidHandle(leftChild)) {
            nodes[leftChild.index].node.parent = parent;
        }
        return true;
    }

    bool setRightChild(NodeHandle parent, NodeHandle rightChild) noexcept {
        // Only validate parent - allow invalid rightChild (leaf node case)
        if (!isValidHandle(parent)) return false;
        
        nodes[parent.index].node.right = rightChild;
        
        // Update the child's parent reference only if child is valid
        if (rightChild.isValid() && isValidHandle(rightChild)) {
            nodes[rightChild.index].node.parent = parent;
        }
        return true;
    }
    
    // Safe deletion
    bool deleteNode(NodeHandle handle) noexcept {
        if (!isValidHandle(handle)) return false;
        
        nodes[handle.index].node = TreeNode{};
        nodes[handle.index].isActive = false;
        freeIndices.push(handle.index);
        
        return true;
    }
    
    void clear() noexcept {
        nodes.clear();
        while (!freeIndices.empty()) freeIndices.pop();
    }
    
    size_t capacity() const noexcept { return maxNodes; }
    size_t getTotalNodeCount() const noexcept { return nodes.size(); }
    size_t getActiveNodeCount() const noexcept { return nodes.size() - freeIndices.size(); }
    size_t getAvailableCapacity() const noexcept { return maxNodes - getActiveNodeCount(); }

    // Prevent copying
    TreeResourceManager(const TreeResourceManager&) = delete;
    TreeResourceManager& operator=(const TreeResourceManager&) = delete;
    
    // Allow moving
    TreeResourceManager(TreeResourceManager&&) = default;
    TreeResourceManager& operator=(TreeResourceManager&&) = default;
};