#include "includes/Strings.hpp"
#include "includes/config.hpp"
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <system_error>

void executable(cmd *inputCmd) {
    const char *userHome = getenv("HOME");
    std::string bindir   = std::string(userHome) + "/.local/bin";

    //sets up bindir
    if (!std::filesystem::exists(bindir) || !std::filesystem::is_directory(bindir)) {
        std::error_code ec;
        std::filesystem::create_directories(bindir, ec);
        if (ec) {
            std::cerr << "Could not create " << bindir << ": " << ec.message() << "\n";
            exit(1);
        }
        std::cerr << "Setting up bin dir.\n";
    }

    // checks if config file is accurate
    if (!std::filesystem::exists(inputCmd->config) || !std::filesystem::is_regular_file(inputCmd->config)) {
        std::cerr << "Invalid config file.\n";
        exit(1);
    }

    //exe part
    std::string bexefile = bindir + "/" + inputCmd->exename; 
 
    // the exename cannot collide with a directory
    if (std::filesystem::is_directory(bexefile)) {
        std::cerr << "You cant use a directory for a config!" << std::endl;
        exit(1);
    }   

    while (std::filesystem::exists(bexefile) && std::filesystem::is_regular_file(bexefile)) {
        char byn = ' ';
        std::cout << "do you want to overwrite: " << inputCmd->exename << "? [y/N] ";
        if (!(std::cin >> byn)) {
            std::cerr << "\nCould not read an answer.\n";
            exit(1);
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        if (std::tolower(byn) == 'y') {
            break;
        } else if (std::tolower(byn) == 'n') {
            std::string exenamein;
            std::cout << "What do you want to call your executable?: ";
            if (!std::getline(std::cin, exenamein)) {
                std::cerr << "\nCould not read a name.\n";
                exit(1);
            }

            inputCmd->exename = exenamein;
            bexefile = bindir + "/" + inputCmd->exename; 
        }   
    }

    // strings needed to create the executable
    std::string exefile          = bindir + "/" + inputCmd->exename; 
    std::string rapidmenucommand = "RapidMenu -c " + shellquote(inputCmd->config);

    std::ofstream fileb (exefile);
    if (!fileb) {
        std::cerr << "Could not write " << exefile << ".\n";
        exit(1);
    }
    fileb << "#!/usr/bin/env bash" << "\n" << std::string(rapidmenucommand).c_str() << "\n";
    std::filesystem::permissions(exefile, std::filesystem::perms::owner_all | std::filesystem::perms::group_all, std::filesystem::perm_options::add);
    fileb.close();
    exit(0);
}
