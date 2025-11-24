#pragma once
#include "TreeNode.hpp"
#include <memory>
#include <vector>
#include <algorithm>

// Single Responsibility: ONLY manages node memory lifetime
class TreeResourceManager {
private:
    std::vector<std::unique_ptr<TreeNode>> nodes;
    std::unordered_map<TreeNode*, size_t> nodeToIndex;

public:

    TreeResourceManager() = default;
    ~TreeResourceManager() = default;
    
    TreeNode* createNode(int value = 0) {
        // Create the node
        nodes.push_back(std::make_unique<TreeNode>());
        nodes.back()->value = value;
        
        // Get the raw pointer
        TreeNode* node = nodes.back().get();
        
        // CRITICAL: Build the index mapping
        nodeToIndex[node] = nodes.size() - 1;
        
        return node;
    }
    
    void deleteNode(TreeNode* node) {
        if (!node) return;
        
        auto it = nodeToIndex.find(node);
        if (it != nodeToIndex.end()) {
            size_t index = it->second;
            
            // Swap with last element for O(1) deletion
            if (index != nodes.size() - 1) {
                std::swap(nodes[index], nodes.back());
                // Update the mapping for the swapped node
                nodeToIndex[nodes[index].get()] = index;
            }
            
            nodes.pop_back();
            nodeToIndex.erase(it);
        }
    }
    
    size_t getManagedNodeCount() const {
        return nodes.size();
    }
    
    // Prevent copying
    TreeResourceManager(const TreeResourceManager&) = delete;
    TreeResourceManager& operator=(const TreeResourceManager&) = delete;
    
    // Allow moving
    TreeResourceManager(TreeResourceManager&&) = default;
    TreeResourceManager& operator=(TreeResourceManager&&) = default;
};