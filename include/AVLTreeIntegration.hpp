#pragma once
#include "AVLTree.hpp"
#include "CommandsParser.hpp"
#include <iostream>
#include <sstream>

class AVLTreeIntegration {
private:
    AVLTree tree;
    CommandsParser parser;

public:
    int executeFromStream(std::istream& input, std::ostream& output, std::ostream& error);
    // Clear state for testing
    void clear() {
        tree.clear();
        parser.clear();
    }
    
    // Get internal state for verification (if needed)
    const AVLTree& getTree() const { return tree; }
    const CommandsParser& getParser() const { return parser; }

private:
    bool executeCommand(const TreeCommandInstance& cmd, std::ostream& output, std::ostream& error);

};