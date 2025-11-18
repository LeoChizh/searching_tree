#include <iostream>
#include "CommandsParser.hpp"

int main(){
    
    CommandsParser cmd;
    
    std::cout << "Enter commands (k=add, m=find_min, n=number_smaller):" << std::endl;
    std::cout << "Example: k 4 k 3 k 2 m 1 n 1" << std::endl;
    std::cout << "Press Ctrl+D (Linux/Mac) or Ctrl+Z (Windows) to finish input" << std::endl;
    
    // Parse from standard input
    cmd.parse_from_stdin();
    
    // Print parsed commands
    std::cout << "\nParsed commands:" << std::endl;
    cmd.print_commands();
    
    return 0;
}
