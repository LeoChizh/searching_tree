#pragma once
#include <cstddef>
#include <cstdint>

// Public handle interface - users include this
struct TreeResourceHandle {
    size_t index;
    uint32_t generation;
    
    // Add proper default constructor
    TreeResourceHandle() : index(INVALID_INDEX), generation(0) {}
    TreeResourceHandle(size_t idx, uint32_t gen) : index(idx), generation(gen) {}
    
    bool operator==(const TreeResourceHandle& other) const {
        return index == other.index && generation == other.generation;
    }
    
    bool operator!=(const TreeResourceHandle& other) const {
        return !(*this == other);
    }
    
    bool isValid() const { return index != INVALID_INDEX; }
    
    static constexpr size_t INVALID_INDEX = ~size_t(0);
};