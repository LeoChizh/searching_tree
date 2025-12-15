#include "AVLTree.hpp"
#include <queue>
#include <sstream>
#include <algorithm>

// Constructors & Destructor
AVLTree::AVLTree() : pool(), root() {}

AVLTree::~AVLTree() {
    clear();
}

AVLTree::AVLTree(const AVLTree& other) : pool(), root() {
    copyAllValues(other);
}

AVLTree::AVLTree(AVLTree&& other) noexcept 
    : pool(std::move(other.pool)), 
      root(std::move(other.root)) {
    other.root = NodeHandle();
}

AVLTree& AVLTree::operator=(const AVLTree& other) {
    if (this != &other) {
        clear();
        copyAllValues(other);
    }
    return *this;
}

AVLTree& AVLTree::operator=(AVLTree&& other) noexcept {
    if (this != &other) {
        clear();
        pool = std::move(other.pool);
        root = std::move(other.root);
        other.root = NodeHandle();
    }
    return *this;
}

// Public interface
void AVLTree::clear() {
    if (root.isValid()) {
        clearSubtreeIterative(root);
        root = NodeHandle();
    }
}

bool AVLTree::empty() const { 
    return !root.isValid(); 
}

bool AVLTree::insert(int value) {
    auto [newRoot, inserted] = insertNode(root, value);
    if (inserted) {
        root = newRoot;
        return true;
    }
    return false;
}

bool AVLTree::remove(int value) {
    auto [newRoot, removed] = removeNode(root, value);
    if (removed) {
        root = newRoot;
        return true;
    }
    return false;
}

bool AVLTree::contains(int value) const {
    return findNode(root, value).isValid();
}

size_t AVLTree::size() const {
    return TreeStructure::getSubtreeSize(pool, root).value_or(0);
}

int AVLTree::getHeight() const {
    return calculateHeight(root);
}

std::string AVLTree::visualize() const {
    if (empty()) return "Empty tree";
    return visualizeSubtree(root, 0);
}

std::optional<int> AVLTree::findNthSmallest(size_t n) const {
    if (n < 1 || n > size()) return std::nullopt;
    return findNthSmallestSubtree(root, n);
}

size_t AVLTree::countSmallerThan(int target) const {
    return countSmallerThanSubtree(root, target);
}

void AVLTree::swap(AVLTree& other) noexcept {
    using std::swap;
    swap(pool, other.pool);
    swap(root, other.root);
}

// Private methods implementation
void AVLTree::copyAllValues(const AVLTree& other) {
    if (other.empty()) return;
    
    std::queue<NodeHandle> q;
    q.push(other.root);
    
    while (!q.empty()) {
        NodeHandle current = q.front();
        q.pop();
        
        // Use TreeStructure to get node value
        auto value = TreeStructure::getNodeValue(other.pool, current);
        if (!value.has_value()) continue;
        
        // Insert value into this tree
        insert(value.value());
        
        // Add children to queue using TreeStructure
        NodeHandle leftChild = TreeStructure::getLeftChild(other.pool, current);
        if (leftChild.isValid()) {
            q.push(leftChild);
        }
        
        NodeHandle rightChild = TreeStructure::getRightChild(other.pool, current);
        if (rightChild.isValid()) {
            q.push(rightChild);
        }
    }
}

void AVLTree::clearSubtreeIterative(NodeHandle nodeHandle) {
    if (!nodeHandle.isValid()) return;
    
    std::queue<NodeHandle> queue;
    queue.push(nodeHandle);
    
    while (!queue.empty()) {
        NodeHandle current = queue.front();
        queue.pop();
        
        NodeHandle leftChild = TreeStructure::getLeftChild(pool, current);
        NodeHandle rightChild = TreeStructure::getRightChild(pool, current);
        
        // Enqueue children before deleting parent
        if (leftChild.isValid()) {
            queue.push(leftChild);
        }
        if (rightChild.isValid()) {
            queue.push(rightChild);
        }
        
        // Delete current node (safe because children are copied to queue)
        pool.deleteNode(current);
    }
}

std::pair<AVLTree::NodeHandle, bool> AVLTree::insertNode(NodeHandle nodeHandle, int value) {
    if (!nodeHandle.isValid()) {
        // Create new node
        NodeHandle newNode = pool.tryCreateNode();
        if (!newNode.isValid()) {
            return {NodeHandle(), false};
        }
        TreeStructure::setNodeValue(pool, newNode, value);
        return {newNode, true};
    }
    
    // Get current node value
    auto nodeValue = TreeStructure::getNodeValue(pool, nodeHandle);
    if (!nodeValue.has_value()) {
        return {NodeHandle(), false};
    }
    
    if (value < nodeValue.value()) {
        NodeHandle leftChild = TreeStructure::getLeftChild(pool, nodeHandle);
        auto [newLeft, inserted] = insertNode(leftChild, value);
        if (inserted) {
            TreeStructure::setLeftChild(pool, nodeHandle, newLeft);
            // Update height and balance
            updateNode(nodeHandle);
            return {balance(nodeHandle), true};
        }
    } else if (value > nodeValue.value()) {
        NodeHandle rightChild = TreeStructure::getRightChild(pool, nodeHandle);
        auto [newRight, inserted] = insertNode(rightChild, value);
        if (inserted) {
            TreeStructure::setRightChild(pool, nodeHandle, newRight);
            // Update height and balance
            updateNode(nodeHandle);
            return {balance(nodeHandle), true};
        }
    }
    
    // Value already exists
    return {nodeHandle, false};
}

std::pair<AVLTree::NodeHandle, bool> AVLTree::removeNode(NodeHandle nodeHandle, int value) {
    if (!nodeHandle.isValid()) {
        return {NodeHandle(), false};
    }
    
    auto nodeValue = TreeStructure::getNodeValue(pool, nodeHandle);
    if (!nodeValue.has_value()) {
        return {NodeHandle(), false};
    }
    
    NodeHandle leftChild = TreeStructure::getLeftChild(pool, nodeHandle);
    NodeHandle rightChild = TreeStructure::getRightChild(pool, nodeHandle);
    
    if (value < nodeValue.value()) {
        auto [newLeft, removed] = removeNode(leftChild, value);
        if (removed) {
            TreeStructure::setLeftChild(pool, nodeHandle, newLeft);
            updateNode(nodeHandle);
            return {balance(nodeHandle), true};
        }
    } else if (value > nodeValue.value()) {
        auto [newRight, removed] = removeNode(rightChild, value);
        if (removed) {
            TreeStructure::setRightChild(pool, nodeHandle, newRight);
            updateNode(nodeHandle);
            return {balance(nodeHandle), true};
        }
    } else {
        // Node to delete found
        if (!leftChild.isValid() || !rightChild.isValid()) {
            // Zero or one child
            NodeHandle child = leftChild.isValid() ? leftChild : rightChild;
            pool.deleteNode(nodeHandle);
            return {child, true};
        } else {
            // Two children: find inorder successor
            NodeHandle successor = findMin(rightChild);
            auto successorValue = TreeStructure::getNodeValue(pool, successor);
            if (!successorValue.has_value()) {
                return {NodeHandle(), false};
            }
            
            // Copy successor's value
            TreeStructure::setNodeValue(pool, nodeHandle, successorValue.value());
            
            // Delete the successor
            auto [newRight, _] = removeNode(rightChild, successorValue.value());
            TreeStructure::setRightChild(pool, nodeHandle, newRight);
            
            updateNode(nodeHandle);
            return {balance(nodeHandle), true};
        }
    }
    
    return {nodeHandle, false};
}

AVLTree::NodeHandle AVLTree::findNode(NodeHandle nodeHandle, int value) const {
    if (!nodeHandle.isValid()) return NodeHandle();
    
    auto nodeValue = TreeStructure::getNodeValue(pool, nodeHandle);
    if (!nodeValue.has_value()) return NodeHandle();
    
    if (value < nodeValue.value()) {
        return findNode(TreeStructure::getLeftChild(pool, nodeHandle), value);
    } else if (value > nodeValue.value()) {
        return findNode(TreeStructure::getRightChild(pool, nodeHandle), value);
    } else {
        return nodeHandle; // Found
    }
}

AVLTree::NodeHandle AVLTree::findMin(NodeHandle nodeHandle) const {
    if (!nodeHandle.isValid()) return NodeHandle();
    
    NodeHandle current = nodeHandle;
    NodeHandle leftChild = TreeStructure::getLeftChild(pool, current);
    
    while (leftChild.isValid()) {
        current = leftChild;
        leftChild = TreeStructure::getLeftChild(pool, current);
    }
    
    return current;
}

void AVLTree::updateNode(NodeHandle nodeHandle) {
    if (!nodeHandle.isValid()) return;
    
    NodeHandle leftChild = TreeStructure::getLeftChild(pool, nodeHandle);
    NodeHandle rightChild = TreeStructure::getRightChild(pool, nodeHandle);
    
    int leftHeight = calculateHeight(leftChild);
    int rightHeight = calculateHeight(rightChild);
    
    TreeStructure::setHeight(pool, nodeHandle, 1 + std::max(leftHeight, rightHeight));
    TreeStructure::setBalanceFactor(pool, nodeHandle, leftHeight - rightHeight);
    TreeStructure::updateSubtreeSize(pool, nodeHandle);
}

AVLTree::NodeHandle AVLTree::balance(NodeHandle nodeHandle) {
    auto balanceFactor = TreeStructure::getBalanceFactor(pool, nodeHandle);
    if (!balanceFactor.has_value()) return nodeHandle;
    
    // Left heavy
    if (balanceFactor.value() > 1) {
        NodeHandle leftChild = TreeStructure::getLeftChild(pool, nodeHandle);
        auto leftBalance = TreeStructure::getBalanceFactor(pool, leftChild);
        if (leftBalance.has_value() && leftBalance.value() < 0) {
            // Left-Right case
            NodeHandle newLeft = rotateLeft(leftChild);
            TreeStructure::setLeftChild(pool, nodeHandle, newLeft);
        }
        return rotateRight(nodeHandle);
    }
    // Right heavy
    else if (balanceFactor.value() < -1) {
        NodeHandle rightChild = TreeStructure::getRightChild(pool, nodeHandle);
        auto rightBalance = TreeStructure::getBalanceFactor(pool, rightChild);
        if (rightBalance.has_value() && rightBalance.value() > 0) {
            // Right-Left case
            NodeHandle newRight = rotateRight(rightChild);
            TreeStructure::setRightChild(pool, nodeHandle, newRight);
        }
        return rotateLeft(nodeHandle);
    }
    
    return nodeHandle;
}

AVLTree::NodeHandle AVLTree::rotateLeft(NodeHandle nodeHandle) {
    NodeHandle rightChild = TreeStructure::getRightChild(pool, nodeHandle);
    if (!rightChild.isValid()) return nodeHandle;
    
    NodeHandle rightLeftChild = TreeStructure::getLeftChild(pool, rightChild);
    
    TreeStructure::setLeftChild(pool, rightChild, nodeHandle);
    TreeStructure::setRightChild(pool, nodeHandle, rightLeftChild);
    
    updateNode(nodeHandle);
    updateNode(rightChild);
    
    return rightChild;
}

AVLTree::NodeHandle AVLTree::rotateRight(NodeHandle nodeHandle) {
    NodeHandle leftChild = TreeStructure::getLeftChild(pool, nodeHandle);
    if (!leftChild.isValid()) return nodeHandle;
    
    NodeHandle leftRightChild = TreeStructure::getRightChild(pool, leftChild);
    
    TreeStructure::setRightChild(pool, leftChild, nodeHandle);
    TreeStructure::setLeftChild(pool, nodeHandle, leftRightChild);
    
    updateNode(nodeHandle);
    updateNode(leftChild);
    
    return leftChild;
}

int AVLTree::calculateHeight(NodeHandle nodeHandle) const {
    if (!nodeHandle.isValid()) return -1;
    
    auto height = TreeStructure::getHeight(pool, nodeHandle);
    return height.has_value() ? height.value() : -1;
}

size_t AVLTree::countNodes(NodeHandle nodeHandle) const {
    if (!nodeHandle.isValid()) return 0;
    
    NodeHandle leftChild = TreeStructure::getLeftChild(pool, nodeHandle);
    NodeHandle rightChild = TreeStructure::getRightChild(pool, nodeHandle);
    
    return 1 + countNodes(leftChild) + countNodes(rightChild);
}

std::string AVLTree::visualizeSubtree(NodeHandle node, int depth) const {
    if (!node.isValid()) return "";
    
    auto value = TreeStructure::getNodeValue(pool, node);
    auto balance = TreeStructure::getBalanceFactor(pool, node);
    auto height = TreeStructure::getHeight(pool, node);
    
    if (!value.has_value() || !balance.has_value() || !height.has_value()) {
        return "";
    }
    
    std::stringstream ss;
    
    // Right child first
    NodeHandle rightChild = TreeStructure::getRightChild(pool, node);
    if (rightChild.isValid()) {
        ss << visualizeSubtree(rightChild, depth + 1);
    }
    
    // Current node with indentation
    for (int i = 0; i < depth; ++i) {
        ss << "    ";
    }
    ss << value.value() << " [H:" << height.value() << " B:" << balance.value() << "]\n";
    
    // Left child last
    NodeHandle leftChild = TreeStructure::getLeftChild(pool, node);
    if (leftChild.isValid()) {
        ss << visualizeSubtree(leftChild, depth + 1);
    }
    
    return ss.str();
}

std::optional<int> AVLTree::findNthSmallestSubtree(NodeHandle node, size_t n) const {
    if (!node.isValid()) return std::nullopt;
    
    // Get left subtree size using the stored value
    NodeHandle leftChild = TreeStructure::getLeftChild(pool, node);
    size_t left_size = TreeStructure::getSubtreeSize(pool, leftChild).value_or(0);
    
    if (n <= left_size) {
        // nth smallest is in left subtree
        return findNthSmallestSubtree(leftChild, n);
    } else if (n == left_size + 1) {
        // Current node is the nth smallest
        return TreeStructure::getNodeValue(pool, node);
    } else {
        // nth smallest is in right subtree
        return findNthSmallestSubtree(
            TreeStructure::getRightChild(pool, node), 
            n - left_size - 1
        );
    }
}

size_t AVLTree::countSmallerThanSubtree(NodeHandle node, int target) const {
    if (!node.isValid()) return 0;
    
    auto current_value = TreeStructure::getNodeValue(pool, node);
    if (!current_value.has_value()) return 0;
    
    if (current_value.value() >= target) {
        // All smaller values are in left subtree
        return countSmallerThanSubtree(TreeStructure::getLeftChild(pool, node), target);
    } else {
        // Current node + all in left subtree + some in right subtree
        NodeHandle leftChild = TreeStructure::getLeftChild(pool, node);
        size_t left_size = TreeStructure::getSubtreeSize(pool, leftChild).value_or(0);
        return left_size + 1 + countSmallerThanSubtree(TreeStructure::getRightChild(pool, node), target);
    }
}