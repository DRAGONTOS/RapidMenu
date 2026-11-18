#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include "includes/Strings.hpp"
#include "includes/config.hpp"
#include "includes/executabel.hpp"

int main(int argc, char **argv, char **envp) {
    cmd inputCmd;
    std::vector<std::string> ARGS{argv, argv + argc};

    if (ARGS.size() < 2) {
        std::cout << USAGE;
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        std::string arg = ARGS[i];
        if (arg.find('-') != 0) {
            continue;
        }

        if (arg == "-h") {
            std::cerr << HELP;
            return 0;
        }

        // config runner
        if (arg == "-c") {
            if (argc != 3) {
                std::cerr << USAGE;
                return 1;
            }

            inputCmd.config = ARGS[2];
            config(&inputCmd);
            return 0;
        }

        // creates executable
        if (arg == "-b") {
            if (argc != 4) {
                std::cerr << USAGE;
                return 1;
            }

            inputCmd.config = ARGS[2];
            inputCmd.exename = ARGS[3];
            executable(&inputCmd);
            return 0;
        }

        std::cerr << USAGE;
        return 1;
    }

    std::cerr << USAGE;
    return 1;
}
