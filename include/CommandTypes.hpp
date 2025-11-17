#pragma once
#include <vector>

enum class TreeCommand { add, find_min, number_smaller };

struct TreeCommandInstance {
    TreeCommand cmd;
    const std::vector<int> values;  // Immutable vector
    
    TreeCommandInstance(TreeCommand c, std::vector<int> vals = {}) 
        : cmd(c), values(std::move(vals)) {}  // Move for efficiency
};