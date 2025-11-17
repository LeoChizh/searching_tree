#include "Commands.hpp"

void Commands::print_commands() {
    for (const auto& instance : commandList) {
        std::cout << commandNames.at(instance.cmd);
        
        // Always print brackets, even if empty - you said this is fine!
        std::cout << " [";
        std::string separator = "";
        for (const auto& value : instance.values) {
            std::cout << separator << value;
            separator = ", ";
        }
        std::cout << "]" << std::endl;
    }
}

void Commands::add_command(TreeCommand cmd, std::vector<int> values) {
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
                
                TreeCommand cmd = char_to_command(cmd_char);
                try {
                    int value = std::stoi(number_str);
                    add_command(cmd, {value});  // This works - {value} creates vector
                    continue; // Successfully processed this token
                } catch (const std::exception& e) {
                    std::cerr << "Invalid number in token: " << token << std::endl;
                }
            }
        }
        
        // Handle standalone commands
        if (token == "k" || token == "m" || token == "n") {
            TreeCommand cmd = char_to_command(token[0]);
            
            // Build values vector
            std::vector<int> values;
            size_t j = i + 1;
            while (j < tokens.size() && is_number(tokens[j])) {
                values.push_back(std::stoi(tokens[j]));
                j++;
            }
            
            if (!values.empty()) i = j - 1;
            
            // SIMPLE: Just pass the vector directly
            add_command(cmd, values);  // No conversion needed!
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


// Helper function to convert character to Command
TreeCommand Commands::char_to_command(char c) {
    switch (c) {
        case 'k': return TreeCommand::add;
        case 'm': return TreeCommand::find_min;
        case 'n': return TreeCommand::number_smaller;
        default: 
            throw std::invalid_argument("Unknown command character: " + std::string(1, c));
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
const std::unordered_map<TreeCommand, std::string> Commands::commandNames = {
    {TreeCommand::add, "add"},
    {TreeCommand::find_min, "find_min"},
    {TreeCommand::number_smaller, "number_smaller"}
};