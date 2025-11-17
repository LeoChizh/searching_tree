#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <cctype>
#include "CommandTypes.hpp"

class Commands {
public:
    void print_commands();
    void add_command(TreeCommand cmd, std::vector<int> values = {});
    void parse_from_stdin();
    void clear();
    size_t size() const;
    
    // Direct access to tree commands - no conversion needed!
    const std::vector<TreeCommandInstance>& get_commands() const { return commandList; }
    
private:
    std::vector<std::string> tokenize_input();
    void parse_tokens(const std::vector<std::string>& tokens);
    bool is_number(const std::string& s);
    std::vector<TreeCommandInstance> commandList;
    static const std::unordered_map<TreeCommand, std::string> commandNames;
};