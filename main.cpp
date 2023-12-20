#include "inc/config.hpp"

int main(int ac, char **av)
{
    if(ac != 2)
         std::cout << "Usage: ./webserv <config_file>" << std::endl;
        return 1;
    
}