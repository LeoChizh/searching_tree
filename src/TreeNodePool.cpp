#include "TreeNodePool.hpp"
#include <algorithm>

TreeNodePool::TreeNodePool(size_t maxNodes) 
    : maxNodes(maxNodes) 
{
    nodes.reserve(std::min(maxNodes, size_t(1000)));
}

TreeNodePool::~TreeNodePool() {
    clear();
}

std::pair<TreeNodePool::NodeHandle, TreeNodePool::CreateResult> 
TreeNodePool::createNode() noexcept {
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

TreeNodePool::NodeHandle TreeNodePool::tryCreateNode() noexcept {
    auto [handle, result] = createNode();
    return handle;
}

TreeNode* TreeNodePool::accessNode(NodeHandle handle) noexcept {
    if (!handle.isValid() || handle.index >= nodes.size()) {
        return nullptr;
    }
    
    auto& entry = nodes[handle.index];
    if (!entry.isActive || entry.generation != handle.generation) {
        return nullptr;
    }
    
    return &entry.node;
}

const TreeNode* TreeNodePool::accessNode(NodeHandle handle) const noexcept {
    return const_cast<TreeNodePool*>(this)->accessNode(handle);
}

bool TreeNodePool::deleteNode(NodeHandle handle) noexcept {
    TreeNode* node = accessNode(handle);
    if (!node) return false;
    
    *node = TreeNode{};  // Reset node data
    nodes[handle.index].isActive = false;
    freeIndices.push(handle.index);
    
    return true;
}

bool TreeNodePool::isValidHandle(NodeHandle handle) const noexcept {
    return accessNode(handle) != nullptr;
}

void TreeNodePool::clear() noexcept {
    nodes.clear();
    while (!freeIndices.empty()) freeIndices.pop();
}

size_t TreeNodePool::capacity() const noexcept { 
    return maxNodes; 
}

size_t TreeNodePool::getTotalNodeCount() const noexcept { 
    return nodes.size(); 
}

size_t TreeNodePool::getActiveNodeCount() const noexcept { 
    return nodes.size() - freeIndices.size(); 
}

size_t TreeNodePool::getAvailableCapacity() const noexcept { 
    return maxNodes - getActiveNodeCount(); 
}