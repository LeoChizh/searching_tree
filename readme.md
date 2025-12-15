# AVL Tree Implementation with Command Parser

A comprehensive C++ implementation of an AVL (self-balancing binary search tree)

## Overview

This project provides a robust AVL tree implementation with the following key features:
- **Self-balancing binary search tree** with O(log n) operations
- **Memory-efficient node pooling** using handle-based system
- **Command parser** for interactive tree operations
- **Comprehensive test suite** with benchmarks
- **STL-compatible interface** with move semantics

## Project Structure

```
├── include/                 # Header files
│   ├── AVLTree.hpp         # Main AVL tree class
│   ├── AVLTreeIntegration.hpp # Integration layer
│   ├── CommandsParser.hpp  # Command parsing
│   ├── TreeNode.hpp        # Tree node structure
│   ├── TreeNodePool.hpp    # Memory management
│   ├── TreeResourceHandle.hpp # Handle system
│   ├── TreeStructure.hpp   # Tree operations
│   └── CommandTypes.hpp    # Command definitions
├── src/                    # Implementation files
│   ├── main.cpp           # Entry point
│   ├── AVLTree.cpp        # AVL tree implementation
│   ├── AVLTreeIntegration.cpp # Integration logic
│   ├── CommandsParser.cpp # Command parsing logic
│   └── TreeNodePool.cpp   # Pool management
├── tests/                  # Comprehensive test suite
│   ├── test_AVLTree.cpp   # Core functionality tests
│   ├── test_commands.cpp  # Command parser tests
│   ├── test_treeNode.cpp  # Node structure tests
│   ├── test_treeNodePool.cpp # Memory pool tests
│   ├── test_treeStructure.cpp # Tree operations tests
│   ├── test_findingSmallest.cpp # Nth smallest tests
│   ├── test_countSmallerThan.cpp # Range counting tests
│   ├── test_subtree_size.cpp # Size tracking tests
│   ├── test_AVLStress.cpp # Stress/performance tests
│   ├── test_AVLBenchmark.cpp # Benchmark comparisons
│   └── test_AVLTreeIntegration.cpp # Integration tests
└── CMakeLists.txt          # Build configuration
```

## Building the Project

### Prerequisites
- C++17 compatible compiler (GCC, Clang, MSVC)
- CMake 3.15+
- Google Test (for tests)

### Build Instructions
```bash
# Clone and build
git clone <repository>
cd SearchingTree
mkdir build && cd build
cmake ..
make

# Run main executable
./SearchingTree

# Run tests
make test
# or
ctest
```

## Usage

### Command Line Interface

The program reads commands from standard input:

#### Available Commands:
- **`k [values...]`** - Add values to the tree
- **`m [n]`** - Find the nth smallest element (1-based)
- **`n [target]`** - Count elements smaller than target

#### Examples:
```bash
# Add values and query
echo "k 50 30 70 20 40 60 80 m 1 m 4 n 25 n 55" | ./SearchingTree
# Output: 20 50 1 4


### Programmatic Usage

```cpp
#include "AVLTree.hpp"

AVLTree tree;

// Basic operations
tree.insert(42);
tree.insert(24);
tree.insert(56);

// Query operations
auto third_smallest = tree.findNthSmallest(3);  // Returns 56
size_t count = tree.countSmallerThan(50);       // Returns 2

// Information
size_t size = tree.size();                      // Returns 3
int height = tree.getHeight();                  // Returns 1
bool contains = tree.contains(24);              // Returns true
```

## Key Features

### 1. Self-Balancing AVL Tree
- Automatic balancing after insertions and deletions
- Guaranteed O(log n) time complexity for all operations
- Height balance property maintained

### 2. Memory Management
- **TreeNodePool**: Efficient node allocation with object pooling
- **Handle System**: Safe memory management without raw pointers
- **Automatic Cleanup**: No memory leaks

### 3. Advanced Operations
- **`findNthSmallest(n)`**: Find the nth smallest element in O(log n) time
- **`countSmallerThan(target)`**: Count elements less than target in O(log n) time
- **Subtree Size Tracking**: Efficient size calculations

### 4. Command Parser
- Flexible input parsing with error handling
- Support for multiple values per command
- Robust error reporting

## Performance

The implementation is optimized for performance:

| Operation | Time Complexity | Space Complexity |
|-----------|-----------------|------------------|
| Insert    | O(log n)        | O(1)             |
| Delete    | O(log n)        | O(1)             |
| Search    | O(log n)        | O(1)             |
| Find Nth  | O(log n)        | O(1)             |
| Count < X | O(log n)        | O(1)             |


## Performance Analysis

### Enhanced Operations (Algorithmic Advantage)

The **subtree size augmentation** enables asymptotically faster operations for order statistics:

| Operation | Your AVLTree | std::set | Theoretical Speedup (n=100k) |
|-----------|--------------|----------|-----------------------------|
| `countSmallerThan()` | **O(log n)** | O(n) | ~5,882x fewer operations |
| `findNthSmallest()` | **O(log n)** | O(n) | ~5,882x fewer operations |

**Measured Results (100,000 elements):**
- `countSmallerThan()`: <1 μs vs std::set: 1,151 μs
- `findNthSmallest()`: <1 μs vs std::set: 1,038 μs

**Why This Matters:** Most standard containers require linear scans for order statistics. Our augmented AVL tree achieves logarithmic time through subtree size tracking.

### Algorithmic Trade-offs: AVL vs Red-Black Trees

The benchmark reveals fundamental differences between balancing strategies:

| Operation | AVL Tree (This Implementation) | Red-Black Tree (std::set) | Primary Reason |
|-----------|--------------------------------|---------------------------|----------------|
| **Insertion** | 10x slower | Baseline | AVL's stricter balance requires more frequent rotations |
| **Deletion** | 4x slower | Baseline | AVL often needs O(log n) rebalancing vs Red-Black's O(1) fix-up |
| **Lookup** | 10% slower | Baseline | Handle system safety overhead outweighs AVL's balance advantage |
| **Order Statistics** | **O(log n)** | O(n) | Our subtree size augmentation enables fast queries |

**Theoretical Confirmation:** These results align perfectly with data structure theory:
- **AVL trees** maintain stricter balance (height difference ≤ 1) for better lookup consistency
- **Red-Black trees** use fewer rotations for faster insertions/deletions
- The performance differences are **inherent to the algorithms**, not implementation flaws

### Safety vs Speed Trade-off Analysis

**Design Philosophy:** This implementation prioritizes safety and advanced features over raw speed:

| Aspect | Your AVLTree | std::set | Rationale |
|--------|--------------|----------|-----------|
| **Memory Safety** | ✅ Handle validation | ❌ Raw pointers | Prevent use-after-free bugs |
| **Memory Management** | ✅ Pool with generational handles | ❌ Default allocator | Predictable, bounded memory usage |
| **Order Statistics** | ✅ O(log n) | ❌ O(n) | Enable advanced queries |
| **Lookup Speed** | -10% | Baseline | Acceptable cost for safety |
| **Insertion Speed** | -10x | Baseline | AVL's strict balancing requirement |
| **Custom Operations** | ✅ Extensible | ❌ Fixed API | Can add new tree operations |

**The 10% Lookup Overhead Explained:**

While AVL trees are theoretically more balanced (should have equal or better lookups), my implementation shows a 10% penalty due to:

1. **Handle Validation**: Every node access checks generational handles for safety
2. **Extra Indirection**: Handle → pool → node vs direct pointer dereference  
3. **Safety Checks**: Validation at each step prevents memory corruption

**Why This Trade-off is Justified:**
- A 10% performance penalty prevents **entire classes of memory bugs**
- Handle system enables **serialization, memory pooling, and safe concurrency**
- Real-world impact: 2.5ms vs 2.3ms for 10,000 lookups
- **In production systems, reliability often outweighs minor speed differences**

### Real-World Application Scenarios

#### Ideal Use Cases:
- **Analytics systems** requiring frequent order statistics (percentiles, rankings)
- **Financial applications** needing range counting and order queries
- **Gaming/leaderboards** with real-time ranking updates
- **Embedded systems** where memory safety is critical
- **Educational tools** demonstrating augmented data structures

#### Less Ideal For:
- Ultra-high-frequency trading (raw speed critical)
- Scenarios with massive insert/delete rates and few queries
- Applications that only need basic set operations

### Benchmark Methodology

-  **Test Environment**: Windows 11, MSYS2 UCRT64 GCC, C++17, full optimizations (`-O3`)
- **Dataset**: 100,000 elements for all tests
- **Validation**: Each test repeated 5 times, averages reported
- **Comparison Baseline**: `std::set<int>` (Red-Black tree implementation)
- **Measurements**: Focus on relative performance, not absolute times

### Key Architectural Insights

1. **Different Design Goals**: std::set optimizes for basic operations; our implementation optimizes for safety and advanced features
2. **Algorithmic Choices Have Costs**: AVL's strict balancing improves lookups but penalizes modifications
3. **Safety Has a Price**: Memory-safe designs typically add 5-20% overhead
4. **Augmentation Enables New Capabilities**: Subtree size tracking enables features std::set cannot provide efficiently

### Conclusion

This implementation demonstrates that data structure design involves balancing multiple concerns: speed, safety, memory usage, and feature set. Our AVL tree makes deliberate trade-offs to excel at order statistics and memory safety, accepting reasonable penalties in modification speed.

## Testing

The project includes extensive testing:

```bash
# Run all tests
make test

# Run specific test groups
./tests/test_AVLTree          # Core functionality
./tests/test_commands         # Command parsing
./tests/test_AVLBenchmark     # Performance benchmarks
./tests/test_AVLStress        # Stress tests
```

### Test Coverage
- Unit tests for all components
- Integration tests for command processing
- Stress tests with large datasets
- Benchmark comparisons against std::set
- Memory leak detection
- Edge case handling

## Design Patterns

- **Resource Handle Pattern**: Safe memory management
- **Object Pool Pattern**: Efficient node allocation
- **Command Pattern**: Input parsing and execution
- **RAII**: Automatic resource management

## Error Handling

- Comprehensive input validation
- Graceful error recovery
- Detailed error messages
- Memory safety guarantees

## AVLTree Class Diagrams

### Class Structure Overview

```mermaid

classDiagram

    class AVLTree {
        -TreeNodePool pool
        -NodeHandle root
        +AVLTree()
        +~AVLTree()
        +AVLTree(const AVLTree&)
        +AVLTree(AVLTree&&)
        +operator=(const AVLTree&)
        +operator=(AVLTree&&)
        +insert(int value) bool
        +remove(int value) bool
        +contains(int value) bool
        +findNthSmallest(size_t n) optional~int~
        +countSmallerThan(int target) size_t
        +size() size_t
        +getHeight() int
        +clear() void
        +empty() bool
        +visualize() string
        +swap(AVLTree&) void
        -insertNode(NodeHandle, int) pair~NodeHandle,bool~
        -removeNode(NodeHandle, int) pair~NodeHandle,bool~
        -balance(NodeHandle) NodeHandle
        -rotateLeft(NodeHandle) NodeHandle
        -rotateRight(NodeHandle) NodeHandle
        -updateNode(NodeHandle) void
    }

    class TreeNodePool {
        +createNode() pair~NodeHandle,CreateResult~
        +tryCreateNode() NodeHandle
        +accessNode(NodeHandle) TreeNode*
        +deleteNode(NodeHandle) bool
        +isValidHandle(NodeHandle) bool
        +clear() void
    }

    class TreeStructure {
        <<namespace>>
        +setLeftChild(TreeNodePool&, NodeHandle, NodeHandle) bool
        +setRightChild(TreeNodePool&, NodeHandle, NodeHandle) bool
        +getNodeValue(const TreeNodePool&, NodeHandle) optional~int~
        +setNodeValue(TreeNodePool&, NodeHandle, int) bool
        +updateSubtreeSize(TreeNodePool&, NodeHandle) bool
    }

    class TreeNode {
        -NodeHandle parent
        -NodeHandle left
        -NodeHandle right
        -int value
        -int balanceFactor
        -int height
        -size_t subtree_size
        +isLeaf() bool
        +isRoot() bool
        +hasLeft() bool
        +hasRight() bool
    }

    class TreeResourceHandle {
        -size_t index
        -uint32_t generation
        +isValid() bool
        +operator==(const TreeResourceHandle&) bool
    }

    AVLTree --> TreeNodePool : uses
    AVLTree --> TreeStructure : uses
    TreeNodePool --> TreeNode : manages
    TreeNodePool --> TreeResourceHandle : creates
    TreeNode --> TreeResourceHandle : contains
    TreeStructure --> TreeNodePool : operates on
```

### Memory Management Flow

```mermaid
sequenceDiagram
    participant Main
    participant AVLTree
    participant TreeNodePool
    participant TreeStructure
    
    Note over Main,TreeStructure: 🔵 Insert Operation Sequence
    
    Main->>+AVLTree: insert(42)
    AVLTree->>+TreeNodePool: tryCreateNode()
    TreeNodePool-->>-AVLTree: NodeHandle
    AVLTree->>+TreeStructure: setNodeValue(pool, handle, 42)
    TreeStructure-->>-AVLTree: success
    AVLTree->>+TreeStructure: setLeftChild(pool, parent, child)
    TreeStructure-->>-AVLTree: success
    AVLTree->>AVLTree: updateNode(handle)
    AVLTree->>AVLTree: balance(handle)
    AVLTree-->>-Main: true
    
    rect rgb(30, 70, 150)
        Note right of AVLTree: 🛠️ Tree Maintenance
        AVLTree->>AVLTree: updateNode(handle)
        AVLTree->>AVLTree: balance(handle)
    end
    
    rect rgb(30, 120, 60)
        Note right of TreeNodePool: 🌱 Node Creation
        AVLTree->>TreeNodePool: tryCreateNode()
        TreeNodePool-->>AVLTree: NodeHandle
    end
    
    rect rgb(150, 100, 30)
        Note right of TreeStructure: 🔧 Tree Operations
        AVLTree->>TreeStructure: setNodeValue(pool, handle, 42)
        AVLTree->>TreeStructure: setLeftChild(pool, parent, child)
    end
```

### AVL Balancing Operations

```mermaid
graph TB
    classDef default fill:#e6f3ff,stroke:#0066cc,stroke-width:2px,color:#003366
    classDef decision fill:#fff2cc,stroke:#d6b656,stroke-width:2px,color:#663300
    classDef action fill:#d5e8d4,stroke:#82b366,stroke-width:2px,color:#2c4a2c
    
    A[Insert/Remove Operation] --> B{Check Balance}
    B -->|Balance > 1| C[Left Heavy]
    B -->|Balance < -1| D[Right Heavy]
    B -->|Balance OK| E[Operation Complete]
    
    C --> F{Left Child Balance}
    F -->|&gt;= 0| G[Left-Left Case]
    F -->|&lt; 0| H[Left-Right Case]
    
    D --> I{Right Child Balance}
    I -->|&lt;= 0| J[Right-Right Case]
    I -->|&gt; 0| K[Right-Left Case]
    
    G --> L[Rotate Right]
    H --> M[Rotate Left then Right]
    J --> N[Rotate Left]
    K --> O[Rotate Right then Left]
    
    L --> E
    M --> E
    N --> E
    O --> E

    class A,E,C,D,G,H,J,K,L,M,N,O default
    class B,F,I decision
```

### Query Operations Flow

```mermaid
flowchart TD
    classDef subgraph1 fill:#e6f3ff,stroke:#0066cc,stroke-width:2px,color:#003366
    classDef subgraph2 fill:#e6ffe6,stroke:#009933,stroke-width:2px,color:#003300
    
    subgraph Subgraph1 [findNthSmallest Algorithm]
        A[findNthSmallestn] --> B{Valid n?}
        B -->|No| C[Return nullopt]
        B -->|Yes| D[Start from root]
        
        D --> E{Get left subtree size}
        E --> F{n <= left_size}
        F -->|Yes| G[Search left subtree]
        F -->|No| H{n == left_size + 1}
        
        H -->|Yes| I[Return current node]
        H -->|No| J[Search right subtree<br>with n - left_size - 1]
        
        G --> K[Recursive call]
        J --> K
    end
    
    subgraph Subgraph2 [countSmallerThan Algorithm]
        L[countSmallerThantarget] --> M{Node valid?}
        M -->|No| N[Return 0]
        M -->|Yes| O{node.value >= target?}
        O -->|Yes| P[Search left subtree]
        O -->|No| Q[Count = left_size + 1<br>+ search right subtree]
        P --> R[Recursive call]
        Q --> R
    end

    class A,B,C,D,E,F,G,H,I,J,K subgraph1
    class L,M,N,O,P,Q,R subgraph2
```



## License

This project is available for educational and research purposes.



