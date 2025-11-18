#pragma once

class Node {
public:
    Node() = default;
    Node(Node* p, Node* l, Node* r, int val) : parent(p), left(l), right(r), value(val) {}
    
    ~Node() {
        parent = nullptr;
    }
    
    // Sets the left child of this node
    // Returns true if successful, false if invalid (self-reference or cycle)
    bool setLeft(Node* newLeft) {
        if (newLeft == this) return false;
        if (newLeft && (newLeft->isAncestor(this) || this->isAncestor(newLeft))) return false;
        
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
        return true;
    }
    
    // Sets the right child of this node
    // Returns true if successful, false if invalid (self-reference or cycle)
    bool setRight(Node* newRight) {
        if (newRight == this) return false;
        if (newRight && (newRight->isAncestor(this) || this->isAncestor(newRight))) return false;
        
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
        return true;
    }
    
    // Checks if the given node is an ancestor of this node
    // Returns true if the node is found in the parent chain
    bool isAncestor(const Node* node) const {
        const Node* current = this;
        while (current) {
            if (current == node) return true;
            current = current->parent;
        }
        return false;
    }
    
    // Removes the specified child node reference
    // Returns true if the child was found and removed, false otherwise
    bool removeChild(Node* child) {
        if (child == nullptr) return false;
        if (child == left) {
            left = nullptr;
            return true;
        } else if (child == right) {
            right = nullptr;
            return true;
        }
        return false;
    }
    
    // Returns the left child node
    Node* getLeft() const { return left; }
    
    // Returns the right child node
    Node* getRight() const { return right; }
    
    // Returns the parent node
    Node* getParent() const { return parent; }

private:
    Node* parent = nullptr;
    Node* left = nullptr;
    Node* right = nullptr;
    int value;
    int balancer;
};