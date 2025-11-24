#include <memory>
#include <vector>
#include <unordered_map>
#include <optional>
#include <TreeNode.hpp>

class TreeResourceManager {
public:
    enum class CreateResult {
        Success,
        TooManyNodes,
        InvalidValue
    };

private:
    std::vector<std::unique_ptr<TreeNode>> nodes;
    std::unordered_map<TreeNode*, size_t> nodeToIndex;
    size_t maxNodes = 1000000;

    bool isValidValue([[maybe_unused]] int value) const {
        // Add your validation logic here
        return true; // Default for now
    }

public:
    // Constructor
    explicit TreeResourceManager(size_t maxNodes = 1000000) 
        : maxNodes(maxNodes) 
    {
        // Optional: pre-allocate memory for better performance
        nodes.reserve(std::min(maxNodes, size_t(1000)));
    }

    // Destructor
    ~TreeResourceManager() {
        // Clear everything in deterministic order
        clear();
    }

    // Primary creation method with clear status
    std::pair<TreeNode*, CreateResult> createNode(int value = 0) noexcept {
        if (nodes.size() >= maxNodes) {
            return {nullptr, CreateResult::TooManyNodes};
        }
        if (!isValidValue(value)) {
            return {nullptr, CreateResult::InvalidValue};
        }
        
        nodes.push_back(std::make_unique<TreeNode>());
        nodes.back()->value = value;
        TreeNode* node = nodes.back().get();
        nodeToIndex[node] = nodes.size() - 1;
        
        return {node, CreateResult::Success};
    }

    // Simple version for cases where you don't care about failure reasons
    TreeNode* tryCreateNode(int value = 0) noexcept {
        auto [node, result] = createNode(value);
        return node; // Returns nullptr on failure
    }
    
    bool deleteNode(TreeNode* node) noexcept {
        if (!node) return false;
        
        auto it = nodeToIndex.find(node);
        if (it == nodeToIndex.end()) {
            return false;
        }
        
        size_t index = it->second;
        if (index != nodes.size() - 1) {
            std::swap(nodes[index], nodes.back());
            nodeToIndex[nodes[index].get()] = index;
        }
        
        nodes.pop_back();
        nodeToIndex.erase(it);
        return true;
    }
    
    bool contains(TreeNode* node) const noexcept {
        return nodeToIndex.find(node) != nodeToIndex.end();
    }
    
    void clear() noexcept {
        nodes.clear();
        nodeToIndex.clear();
    }
    
    size_t capacity() const noexcept { 
        return maxNodes; 
    }
    
    size_t getManagedNodeCount() const noexcept { 
        return nodes.size(); 
    }
    
    size_t getAvailableCapacity() const noexcept {
        return maxNodes - nodes.size();
    }

    // Prevent copying
    TreeResourceManager(const TreeResourceManager&) = delete;
    TreeResourceManager& operator=(const TreeResourceManager&) = delete;
    
    // Allow moving
    TreeResourceManager(TreeResourceManager&&) = default;
    TreeResourceManager& operator=(TreeResourceManager&&) = default;
};