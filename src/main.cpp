#include <iostream>
#include "Commands.hpp"

int main(){
    
    Commands cmd;
    
    // Add some commands with different numbers of parameters
    cmd.add_command(Commands::Command::add, {10, 20});
    cmd.add_command(Commands::Command::find_min, {5});
    cmd.add_command(Commands::Command::number_smaller, {1, 2, 3, 4});
    cmd.add_command(Commands::Command::add); // No parameters
    
    // Print all commands
    cmd.print_commands();
    
    return 0;
}
