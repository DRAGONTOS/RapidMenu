#include "includes/Strings.hpp"
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <limits>
#include <string>
#include <sys/stat.h>

void executable(cmd *inputCmd) {
    const char *bexe     = nullptr;
    const char *userHome = getenv("HOME");
    std::cout << inputCmd->config << "\n";

    char byn;
    
    std::string bconfigin = inputCmd->config;
    std::string bindir    = std::string(userHome) + "/.local/bin";
    
    //sets up bindir
    if (std::filesystem::exists(bindir) && std::filesystem::is_directory(bindir)) {
    } else {
        mkdir(bindir.c_str(), 0777);
        std::cerr << "Setting up bin dir.\n";
    }
    
    // checks if config file is accurate
    if (std::filesystem::exists(inputCmd->config) && std::filesystem::is_regular_file(inputCmd->config)) {
    } else {
        std::cerr << "Invalid config file.\n"; 
        exit(1);
    }

    //exe part
    std::string bexefile = bindir + "/" + inputCmd->exename; 
    std::cout << bexefile << "\n";
 
    // not sure why this isnt used might look into it at a later date.
    if (std::filesystem::is_directory(bexefile)) {
        std::cerr << "You cant use a directory for a config!";
        exit(1);
    }   

    while (std::filesystem::exists(bexefile) && std::filesystem::is_regular_file(bexefile)) {
        std::cout << "do you want to overwrite: " << inputCmd->exename << "? [y/N] ";
        std::cin >> byn;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        if (std::tolower(byn) == 'y') {
            break;
        } else if (std::tolower(byn) == 'n') {
            std::string exenamein;
            std::cout << "What do you want to call your executable?: ";
            std::getline(std::cin, exenamein);

            inputCmd->exename = exenamein;
            bexefile = bindir + "/" + inputCmd->exename; 
        }   
    }

    // strings needed to create the executable
    std::string exefile          = bindir + "/" + inputCmd->exename; 
    std::string rapidmenucommand = "RapidMenu -c " + inputCmd->config;

    std::ofstream fileb (exefile);
    fileb << "#!/usr/bin/env bash" << "\n" << std::string(rapidmenucommand).c_str() << "\n";
    std::filesystem::permissions(bexefile, std::filesystem::perms::owner_all | std::filesystem::perms::group_all, std::filesystem::perm_options::add);
    fileb.close();
    exit(0);
}
