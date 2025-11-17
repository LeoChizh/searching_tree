#include "Commands.hpp"

// CommandInstance constructor definition - must be in the same translation unit
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

// Stage 1: Tokenize input
std::vector<std::string> Commands::tokenize_input() {
    std::vector<std::string> tokens;
    std::string line;
    
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        
        std::istringstream iss(line);
        std::string token;
        
        while (iss >> token) {
            tokens.push_back(token);
        }
    }
    
    return tokens;
}

// Stage 2: Parse tokens into commands
void Commands::parse_tokens(const std::vector<std::string>& tokens) {
    for (size_t i = 0; i < tokens.size(); ++i) {
        const std::string& token = tokens[i];
        
        // Check for command with attached number (like "k4")
        if (token.length() > 1 && (token[0] == 'k' || token[0] == 'm' || token[0] == 'n')) {
            // Check if the rest is a valid number
            bool is_attached_number = true;
            for (size_t j = 1; j < token.length(); ++j) {
                if (!std::isdigit(token[j]) && !(j == 1 && token[j] == '-')) {
                    is_attached_number = false;
                    break;
                }
            }
            
            if (is_attached_number) {
                // Parse command with attached number
                char cmd_char = token[0];
                std::string number_str = token.substr(1);
                
                Command cmd = char_to_command(cmd_char);
                try {
                    int value = std::stoi(number_str);
                    add_command(cmd, {value});
                    continue; // Successfully processed this token
                } catch (const std::exception& e) {
                    std::cerr << "Invalid number in token: " << token << std::endl;
                }
            }
        }
        
        // Handle standalone commands
        if (token == "k" || token == "m" || token == "n") {
            Command cmd = char_to_command(token[0]);
            
            // Check if next token is a number
            if (i + 1 < tokens.size()) {
                const std::string& next_token = tokens[i + 1];
                if (is_number(next_token)) {
                    try {
                        int value = std::stoi(next_token);
                        add_command(cmd, {value});
                        i++; // Skip the number token since we used it
                        continue;
                    } catch (const std::exception& e) {
                        std::cerr << "Invalid number: " << next_token << std::endl;
                    }
                }
            }
            
            // No number provided, add command without value
            add_command(cmd);
        } else {
            std::cerr << "Unknown token: " << token << std::endl;
        }
    }
}

// Combined method for easy use
void Commands::parse_from_stdin() {
    std::vector<std::string> tokens = tokenize_input();
    parse_tokens(tokens);
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

// Helper function to convert character to Command
Commands::Command Commands::char_to_command(char c) {
    switch (c) {
        case 'k': return Command::add;
        case 'm': return Command::find_min;
        case 'n': return Command::number_smaller;
        default: return Command::add; // Should not happen with validation
    }
}

// Helper function to check if string is a number
bool Commands::is_number(const std::string& s) {
    if (s.empty()) return false;
    
    size_t start = 0;
    if (s[0] == '-') {
        if (s.length() == 1) return false; // Just "-" is not a number
        start = 1;
    }
    
    for (size_t i = start; i < s.length(); ++i) {
        if (!std::isdigit(s[i])) return false;
    }
    
    return true;
}

// Static member definition
const std::unordered_map<Commands::Command, std::string> Commands::commandNames = {
    {Commands::Command::add, "add"},
    {Commands::Command::find_min, "find_min"},
    {Commands::Command::number_smaller, "number_smaller"}
};