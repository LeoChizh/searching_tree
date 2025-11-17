#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <cctype>

// Forward declaration for friend class
class CommandsTest_PublicInterface;
class CommandsTest_PrivateImplementation;

class Commands {
    // Make test classes friends to access private members
    friend class CommandsTest_PublicInterface;
    friend class CommandsTest_PrivateImplementation;
    
public:
    enum class Command { add, find_min, number_smaller };

    void print_commands();
    void add_command(Command cmd, std::initializer_list<int> values = {});
    void parse_from_stdin();
    void clear();
    size_t size() const;
    const std::vector<int>& get_command_values(size_t index) const;
    Command get_command_type(size_t index) const;
    
private:
    // Remove the CommandInstance constructor definition from here
    struct CommandInstance {
        Command cmd;
        std::vector<int> values;
        CommandInstance(Command c, std::initializer_list<int> vals = {}); // Declaration only
    };
    
    std::vector<std::string> tokenize_input();
    void parse_tokens(const std::vector<std::string>& tokens);
    Command char_to_command(char c);
    bool is_number(const std::string& s);
    
    std::vector<CommandInstance> commandList;
    
    // Add 'static' here
    static const std::unordered_map<Command, std::string> commandNames;
};