#include "CommandsParser.hpp"

void CommandsParser::print_commands() {
    for (const auto& instance : commandList) {
        std::cout << commandNames.at(instance.cmd);
        
        // Always print brackets, even if empty
        std::cout << " [";
        std::string separator = "";
        for (const auto& value : instance.values) {
            std::cout << separator << value;
            separator = ", ";
        }
        std::cout << "]" << std::endl;
    }
}

void CommandsParser::add_command(TreeCommand cmd, std::vector<int> values) {
    commandList.emplace_back(cmd, values);
}

// Stage 1: Tokenize input
std::vector<std::string> CommandsParser::tokenize_input() {
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
// Stage 2 parse tokens
void CommandsParser::parse_tokens(const std::vector<std::string>& tokens) {
    for (size_t i = 0; i < tokens.size(); ++i) {
        const std::string& token = tokens[i];
        
        // Handle valid commands
        if (token == "k" || token == "m" || token == "n") {
            TreeCommand cmd;
            if (token == "k") {
                cmd = TreeCommand::add;
            } else if (token == "m") {
                cmd = TreeCommand::find_min;
            } else if (token == "n") {
                cmd = TreeCommand::number_smaller;
            }
            
            std::vector<int> values;
            
            // Collect consecutive numbers after the command
            size_t j = i + 1;
            while (j < tokens.size() && is_number(tokens[j])) {
                values.push_back(std::stoi(tokens[j]));
                j++;
            }
            
            // Add the command with its values
            add_command(cmd, values);
            
            // Update index to skip processed values
            if (j > i + 1) {
                i = j - 1;
            }
            
        } else if (is_number(token)) {
            // Number without preceding command - report error but continue
            errors_count++;
            std::cerr << "Error: Number without preceding command: " << token << std::endl;
        } else {
            // Unknown token - report error but continue
            errors_count++;
            std::cerr << "Error: Unknown token: " << token << std::endl;
        }
    }
    return errors_count;
}

// Combined method for easy use
void CommandsParser::parse_from_stdin() {
    std::vector<std::string> tokens = tokenize_input();
    return parse_tokens(tokens);
}

// Clear all commands
void CommandsParser::clear() {
    commandList.clear();
}

// Get number of commands (useful for testing)
size_t CommandsParser::size() const {
    return commandList.size();
}


// Helper function to convert character to Command
TreeCommand CommandsParser::char_to_command(char c) {
    switch (c) {
        case 'k': return TreeCommand::add;
        case 'm': return TreeCommand::find_min;
        case 'n': return TreeCommand::number_smaller;
        default: 
            throw std::invalid_argument("Unknown command character: " + std::string(1, c));
    }
}

// Helper function to check if string is a number
bool CommandsParser::is_number(const std::string& s) {
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
const std::unordered_map<TreeCommand, std::string> CommandsParser::commandNames = {
    {TreeCommand::add, "add"},
    {TreeCommand::find_min, "find_min"},
    {TreeCommand::number_smaller, "number_smaller"}
};