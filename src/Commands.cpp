// Commands.cpp
#include "Commands.hpp"


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

// Fixed parser: handles "k 4 k3 k 2" format correctly
void Commands::parse_from_stdin() {
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        
        std::istringstream iss(line);
        std::string token;
        
        while (iss >> token) {
            // Check if token starts with a command character
            if (token.length() >= 1 && (token[0] == 'k' || token[0] == 'm' || token[0] == 'n')) {
                Command cmd;
                
                switch (token[0]) {
                    case 'k': cmd = Command::add; break;
                    case 'm': cmd = Command::find_min; break;
                    case 'n': cmd = Command::number_smaller; break;
                    default: continue; // Should never happen
                }
                
                // Check if there's a number attached to the command (like "k3")
                std::vector<int> values;
                
                if (token.length() > 1) {
                    // Try to parse the rest as a number
                    std::string number_part = token.substr(1);
                    try {
                        int value = std::stoi(number_part);
                        values.push_back(value);
                    } catch (const std::exception& e) {
                        // If it's not a valid number, ignore it
                    }
                } else {
                    // Command without attached number, try to read next token as number
                    if (iss.peek() != EOF && std::isdigit(iss.peek())) {
                        int value;
                        if (iss >> value) {
                            values.push_back(value);
                        }
                    }
                }
                
                // Add the command
                if (!values.empty()) {
                    add_command(cmd, {values[0]});
                } else {
                    add_command(cmd);
                }
            } else {
                std::cerr << "Unknown command: " << token << std::endl;
            }
        }
    }
}

// Alternative: Even simpler and more robust parser
void Commands::parse_commands_robust() {
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        
        for (size_t i = 0; i < line.length(); ++i) {
            char c = line[i];
            if (c == 'k' || c == 'm' || c == 'n') {
                Commands::Command cmd;
                switch (c) {
                    case 'k': cmd = Command::add; break;
                    case 'm': cmd = Command::find_min; break;
                    case 'n': cmd = Command::number_smaller; break;
                    default: continue;
                }
                
                // Look for numbers after the command
                std::vector<int> values;
                size_t j = i + 1;
                
                // Skip whitespace
                while (j < line.length() && std::isspace(line[j])) {
                    j++;
                }
                
                // Parse number if present
                if (j < line.length() && std::isdigit(line[j])) {
                    std::string number_str;
                    while (j < line.length() && (std::isdigit(line[j]) || line[j] == '-')) {
                        number_str += line[j];
                        j++;
                    }
                    try {
                        int value = std::stoi(number_str);
                        values.push_back(value);
                    } catch (const std::exception& e) {
                        // Ignore invalid numbers
                    }
                }
                
                // Add the command
                if (!values.empty()) {
                    Commands::add_command(cmd, {values[0]});
                } else {
                    add_command(cmd);
                }
                
                i = j - 1; // Continue from after the number
            }
        }
    }
}
    
// Clear all commands
void Commands::clear() {
    commandList.clear();
}
    
// Get number of commands (useful for testing)
size_t Commands::size() const {
    return commandList.size();
}
    
// Get command at index (for testing)
const std::vector<int>& Commands::get_command_values(size_t index) const {
    return commandList[index].values;
}
    
// Get command type at index (for testing)
Commands::Command Commands::get_command_type(size_t index) const {
    return commandList[index].cmd;
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