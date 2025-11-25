#include <iostream>
#include "CommandsParser.hpp"
#include "AVLTree.hpp"

int main(){

    AVLTree tree;

    // Test left-heavy scenario
    tree.insert(50);
    tree.insert(40);
    tree.insert(30);
    std::cout << "After left-heavy insertion:\n" << tree.visualize() << std::endl;

    tree.clear();

    // Test right-heavy scenario  
    tree.insert(10);
    tree.insert(20);
    tree.insert(30);
    std::cout << "After right-heavy insertion:\n" << tree.visualize() << std::endl;

    tree.clear();

    // Test complex scenario
    tree.insert(50);
    tree.insert(30);
    tree.insert(70);
    tree.insert(20);
    tree.insert(40);
    tree.insert(60);
    tree.insert(80);
    tree.insert(10);
    tree.insert(25);
    std::cout << "Complex tree:\n" << tree.visualize() << std::endl;

    tree.insert(5);  // This might trigger a rotation
    std::cout << "After inserting 5:\n" << tree.visualize() << std::endl;
    
    CommandsParser cmd;
    
    std::cout << "Enter commands (k=add, m=find_min, n=number_smaller):" << std::endl;
    std::cout << "Example: k 4 k 3 k 2 m 1 n 1" << std::endl;
    std::cout << "Press Ctrl+D (Linux/Mac) or Ctrl+Z (Windows) to finish input" << std::endl;
    
    // Parse from standard input
    int error_count = cmd.parse_from_stdin();
    
    // Print parsed commands
    std::cout << "\nParsed commands with " << error_count << " error(s):" << std::endl;
    cmd.print_commands();
    
    return 0;
}
