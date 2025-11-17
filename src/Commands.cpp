// Commands.cpp
#include "Commands.hpp"
#include <iostream>

Commands::CommandInstance::CommandInstance(Command c, std::initializer_list<int> vals) 
    : cmd(c), values(vals) {}

void Commands::print_commands() {
    for (const auto& instance : commandList) {
        std::cout << commandNames.at(instance.cmd);
        
        if (!instance.values.empty()) {
            std::cout << " [";
            std::string separator = "";
            for (const auto& value : instance.values) {
                std::cout << separator << value;
                separator = ", ";
            }
            std::cout << "]";
        }
        std::cout << std::endl;
    }
}

void Commands::add_command(Command cmd, std::initializer_list<int> values) {
    commandList.emplace_back(cmd, values);
}

// Static member definition
const std::unordered_map<Commands::Command, std::string> Commands::commandNames = {
    {Commands::Command::add, "add"},
    {Commands::Command::find_min, "find_min"},
    {Commands::Command::number_smaller, "number_smaller"}
};