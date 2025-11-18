#pragma once
#include <memory>
#include <vector>
#include <unordered_set>
#include <stdexcept>
#include <algorithm>
#include "Node.hpp"

class Tree {
public:
    Tree() = default;
    
    ~Tree() {
        clear();  // Non-recursive destruction
    }
    
    // Prevent copying unless explicitly implemented
    Tree(const Tree&) = delete;
    Tree& operator=(const Tree&) = delete;
    
    // Move operations
    Tree(Tree&& other) noexcept = default;
    Tree& operator=(Tree&& other) noexcept = default;
    
    // Create node with validation
    Node* createNode(Node* parent = nullptr) {
        auto node = std::make_unique<Node>();
        Node* rawPtr = node.get();
        
        if (parent) {
            // Decide where to insert (left or right)
            if (!parent->getLeft()) {
                parent->setLeft(rawPtr);
            } else if (!parent->getRight()) {
                parent->setRight(rawPtr);
            } else {
                throw std::runtime_error("Parent node already has two children");
            }
        }
        
        nodes.push_back(std::move(node));
        return rawPtr;
    }
    
    // Complex tree operations
    Node* insertLeft(Node* parent, Node* existingNode = nullptr) {
        if (!parent) throw std::invalid_argument("Parent cannot be null");
        if (parent->getLeft()) throw std::runtime_error("Left child already exists");
        
        Node* newNode = existingNode ? adoptNode(existingNode) : createNode();
        parent->setLeft(newNode);
        return newNode;
    }
    
    Node* insertRight(Node* parent, Node* existingNode = nullptr) {
        if (!parent) throw std::invalid_argument("Parent cannot be null");
        if (parent->getRight()) throw std::runtime_error("Right child already exists");
        
        Node* newNode = existingNode ? adoptNode(existingNode) : createNode();
        parent->setRight(newNode);
        return newNode;
    }
    
    // Advanced removal with options
    enum class RemovalPolicy {
        Detach,     // Just remove from tree, keep node
        Delete,     // Remove and delete node
        Recursive   // Remove entire subtree (with careful iteration)
    };
    
    void removeNode(Node* node, RemovalPolicy policy = RemovalPolicy::Delete) {
        if (!node || !ownsNode(node)) return;
        
        // Detach from parent first
        if (node->getParent()) {
            node->getParent()->removeChild(node);
        }
        
        if (policy == RemovalPolicy::Recursive) {
            removeSubtreeIterative(node);
        } else if (policy == RemovalPolicy::Delete) {
            deleteNode(node);
        }
        // For Detach policy, we just leave the node in the container but detached
    }
    
    // Non-recursive subtree deletion
    void removeSubtreeIterative(Node* root) {
        if (!root || !ownsNode(root)) return;
        
        std::vector<Node*> toDelete;
        std::vector<Node*> stack = {root};
        
        // Collect all nodes in subtree iteratively
        while (!stack.empty()) {
            Node* current = stack.back();
            stack.pop_back();
            
            toDelete.push_back(current);
            
            // Push children in reverse order (for pre-order collection)
            if (current->getRight()) stack.push_back(current->getRight());
            if (current->getLeft()) stack.push_back(current->getLeft());
        }
        
        // Delete nodes (detach first to break links)
        for (Node* node : toDelete) {
            if (node->getParent()) {
                node->getParent()->removeChild(node);
            }
            deleteNode(node);
        }
    }
    
    // Validation
    bool isValid() const {
        if (nodes.empty()) return true;
        
        // Check all nodes for validity
        for (const auto& node : nodes) {
            if (!node->isValid()) return false;
        }
        
        // Check for single root
        return std::count_if(nodes.begin(), nodes.end(),
            [](const std::unique_ptr<Node>& node) {
                return node->getParent() == nullptr;
            }) == 1;
    }
    
    void clear() {
        // Clear in reverse order to minimize dangling pointers
        for (auto it = nodes.rbegin(); it != nodes.rend(); ++it) {
            if ((*it)->getParent()) {
                (*it)->getParent()->removeChild(it->get());
            }
        }
        nodes.clear();
    }
    
    bool ownsNode(Node* node) const {
        return std::find_if(nodes.begin(), nodes.end(),
            [node](const std::unique_ptr<Node>& ptr) {
                return ptr.get() == node;
            }) != nodes.end();
    }

private:
    std::vector<std::unique_ptr<Node>> nodes;
    
    Node* adoptNode(Node* node) {
        // Implementation to move node between trees
        // (would need to remove from source tree first)
        throw std::runtime_error("Node adoption not implemented");
    }
    
    void deleteNode(Node* node) {
        auto it = std::find_if(nodes.begin(), nodes.end(),
            [node](const std::unique_ptr<Node>& ptr) {
                return ptr.get() == node;
            });
        if (it != nodes.end()) {
            nodes.erase(it);
        }
    }
};