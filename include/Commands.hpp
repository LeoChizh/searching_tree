#pragma once
#include <vector>
#include <string>
#include <unordered_map>

class Commands {
public:
    enum class Command { add, find_min, number_smaller };

    void print_commands();
    void add_command(Command cmd, std::initializer_list<int> values = {});
    
private:  
    struct CommandInstance {
        Command cmd;
        std::vector<int> values;
        CommandInstance(Command c, std::initializer_list<int> vals = {});
    };
    
    std::vector<CommandInstance> commandList;
    static const std::unordered_map<Command, std::string> commandNames;
};