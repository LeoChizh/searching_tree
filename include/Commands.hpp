#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <sstream>  // Add this for std::istringstream

class Commands {
public:
    enum class Command { add, find_min, number_smaller };

    void print_commands();
    std::vector<std::string> tokenize_input();
    void parse_tokens(const std::vector<std::string> &tokens);
    void parse_from_stdin();
    void clear();
    size_t size() const;
    const std::vector<int> &get_command_values(size_t index) const;
    Command get_command_type(size_t index) const;
    void add_command(Command cmd, std::initializer_list<int> values = {});



private:  
    struct CommandInstance {
        Command cmd;
        std::vector<int> values;
        CommandInstance(Command c, std::initializer_list<int> vals = {});
    };
    Commands::Command char_to_command(char c);
    bool is_number(const std::string &s);
    std::vector<CommandInstance> commandList;
    static const std::unordered_map<Command, std::string> commandNames;
};