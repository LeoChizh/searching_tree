#pragma once
#include <stdexcept>

class Node {
public:
    Node() = default;
    Node(Node* p, Node* l, Node* r) : parent(p), left(l), right(r) {}
    
    // CRITICAL: Non-recursive destructor
    ~Node() {
        // Only clear parent links, don't delete children recursively
        parent = nullptr;
        // Note: we DON'T do: delete left; delete right;
    }
    
    // Safe setter methods with validation
    void setLeft(Node* newLeft) {
        if (newLeft == this) {
            throw std::invalid_argument("Cannot set self as left child");
        }
        if (newLeft && newLeft->isAncestor(this)) {
            throw std::invalid_argument("Would create cycle in tree");
        }
        
        // Properly detach existing left child
        if (left && left->parent == this) {
            left->parent = nullptr;
        }
        
        left = newLeft;
        if (left) {
            if (left->parent) {
                left->parent->removeChild(left);
            }
            left->parent = this;
        }
    }
    
    void setRight(Node* newRight) {
        if (newRight == this) {
            throw std::invalid_argument("Cannot set self as right child");
        }
        if (newRight && newRight->isAncestor(this)) {
            throw std::invalid_argument("Would create cycle in tree");
        }
        
        if (right && right->parent == this) {
            right->parent = nullptr;
        }
        
        right = newRight;
        if (right) {
            if (right->parent) {
                right->parent->removeChild(right);
            }
            right->parent = this;
        }
    }
    
    // Complex validation methods
    bool isAncestor(const Node* node) const {
        const Node* current = this;
        while (current) {
            if (current == node) return true;
            current = current->parent;
        }
        return false;
    }
    
    bool isDescendant(const Node* node) const {
        return node ? node->isAncestor(this) : false;
    }
    
    // Safe child removal
    void removeChild(Node* child) {
        if (child == left) {
            left = nullptr;
        } else if (child == right) {
            right = nullptr;
        }
    }
    
    // Getters
    Node* getLeft() const { return left; }
    Node* getRight() const { return right; }
    Node* getParent() const { return parent; }
    
    // State validation
    bool isValid() const {
        // Check for cycles
        if (left && (left == this || left->isAncestor(this))) return false;
        if (right && (right == this || right->isAncestor(this))) return false;
        
        // Check parent consistency
        if (left && left->parent != this) return false;
        if (right && right->parent != this) return false;
        
        return true;
    }

private:
    Node* parent = nullptr;
    Node* left = nullptr;
    Node* right = nullptr;
};