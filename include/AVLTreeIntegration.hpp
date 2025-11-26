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
    

    int executeFromStream(std::istream& input, std::ostream& output, std::ostream& error) {
        // Save original buffers
        auto old_cin_buf = std::cin.rdbuf();
        auto old_cerr_buf = std::cerr.rdbuf();
        
        // Redirect cin to our input stream
        std::cin.rdbuf(input.rdbuf());
        // Redirect cerr to our error stream to capture parser errors
        std::cerr.rdbuf(error.rdbuf());
        
        // Parse commands
        int parseErrors = parser.parse_from_stdin();
        
        // Restore original buffers
        std::cin.rdbuf(old_cin_buf);
        std::cerr.rdbuf(old_cerr_buf);
        
        // Execute commands
        int executionErrors = 0;
        for (const auto& command : parser.get_commands()) {
            if (!executeCommand(command, output, error)) {
                executionErrors++;
            }
        }
    
        return parseErrors + executionErrors;
    }
    
    // Clear state for testing
    void clear() {
        tree.clear();
        parser.clear();
    }
    
    // Get internal state for verification (if needed)
    const AVLTree& getTree() const { return tree; }
    const CommandsParser& getParser() const { return parser; }

private:
   bool executeCommand(const TreeCommandInstance& cmd, std::ostream& output, std::ostream& error) {
    switch (cmd.cmd) {
        case TreeCommand::add:
            // add must have at least 1 value
            if (cmd.values.empty()) {
                error << "Error: add requires at least one value, got 0" << std::endl;
                return false;
            } else {
                for (int value : cmd.values) {
                    tree.insert(value);
                }
            }
            return true;
            
        case TreeCommand::find_min:
            // find_min must have exactly 1 value
            if (cmd.values.size() != 1) {
                error << "Error: find_min requires exactly one value, got " << cmd.values.size() << std::endl;
                output << std::endl; // Still output empty line for consistency
                return false;
            } else {
                auto result = tree.findNthSmallest(cmd.values[0]);
                if (result.has_value()) {
                    output << result.value() << std::endl;
                } else {
                    output << std::endl;
                }
            }
            return true;
            
        case TreeCommand::number_smaller:
            // number_smaller must have exactly 1 value
            if (cmd.values.size() != 1) {
                error << "Error: number_smaller requires exactly one value, got " << cmd.values.size() << std::endl;
                output << 0 << std::endl; // Still output 0 for consistency
                return false;
            } else {
                size_t count = tree.countSmallerThan(cmd.values[0]);
                output << count << std::endl;
            }
            return true;
            
        default:
            error << "Error: Unknown command type" << std::endl;
            return false;
    }
  }
};