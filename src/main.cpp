#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include "includes/Strings.hpp"
#include "includes/config.hpp"
#include "includes/executabel.hpp"

int main(int argc, char **argv, char **envp) {
    // cmd inputCmd;
    cmd inputCmd;
    std::vector<std::string> ARGS{argv, argv + argc};
    for (int i = 0; i < argc; ++i) {
        ARGS[i] = std::string{argv[i]};
    }

    if (ARGS.size() < 2) {
        std::cout << USAGE;
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.find('-') == 0) {
            if (ARGS[i] == "-h") {
                if (argc < 3 || argv[2][0] == '-') {
                    std::cerr << HELP;
                    return 1;
                }
                std::cerr << HELP;
                return 1;
            }

            // config runner
            if (ARGS[i] == "-c") {
                if (argc < 3 || argv[2][0] == '-') {
                    std::cerr << USAGE;
                    return 1;
                }
            
                if (argc == 3) {
                    inputCmd.config = ARGS[1 + 1];
                } else {
                    std::cerr << HELP;
                    return 1;
                }

                config(&inputCmd);
                return 1;
            } 

            // creates executable
            if (ARGS[i] == "-b") {
                if (argc < 3 || argv[2][0] == '-') {
                    std::cerr << USAGE;
                    return 1;
                }
                
                if (argc == 4) {
                    inputCmd.config  = ARGS[1 + 1];
                    inputCmd.exename = ARGS[1 + 2];
                } else {
                    std::cerr << HELP;
                    return 1;
                }

                std::cout << "config: " << inputCmd.config << " bindir: " << inputCmd.exename << "\n";

                executable(&inputCmd);
                return 1;
            }
        }
    }

                     
    std::cout << "\nArguments: " << argc << std::endl;
    return 0;
}
