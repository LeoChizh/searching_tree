#include "AVLTreeIntegration.hpp"


int AVLTreeIntegration::executeFromStream(std::istream& input, std::ostream& output, std::ostream& error) {
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
    
    // Execute commands with proper spacing
    int executionErrors = 0;
    bool firstOutput = true;
    
    for (const auto& command : parser.get_commands()) {
        // Create a stringstream to capture output from this command
        std::ostringstream commandOutput;
        
        if (executeCommand(command, commandOutput, error)) {
            // If command produced output, handle spacing
            std::string outputStr = commandOutput.str();
            if (!outputStr.empty()) {
                if (!firstOutput) {
                    output << " "; // Add space before if not first output
                }
                output << outputStr;
                firstOutput = false;
            }
        } else {
            executionErrors++;
        }
    }
    
    return parseErrors + executionErrors;
}

bool AVLTreeIntegration::executeCommand(const TreeCommandInstance& cmd, std::ostream& output, std::ostream& error) {
    switch (cmd.cmd) {
        case TreeCommand::add:
            if (cmd.values.empty()) {
                error << "Error: add requires at least one value, got 0" << std::endl;
                return false;
            }
            for (int value : cmd.values) {
                tree.insert(value);
            }
            return true;
            
        case TreeCommand::find_min:
            if (cmd.values.size() != 1) {
                error << "Error: find_min requires exactly one value, got " << cmd.values.size() << std::endl;
                return false;
            }
            {
                auto result = tree.findNthSmallest(cmd.values[0]);
                if (result.has_value()) {
                    output << result.value();
                    return true;
                }
                // Don't output anything for no result
                return true;
            }
            
        case TreeCommand::number_smaller:
            if (cmd.values.size() != 1) {
                error << "Error: number_smaller requires exactly one value, got " << cmd.values.size() << std::endl;
                output << "0"; // Output 0 for error case
                return false;
            }
            {
                size_t count = tree.countSmallerThan(cmd.values[0]);
                output << count;
                return true;
            }
            
        default:
            error << "Error: Unknown command type" << std::endl;
            return false;
    }
}