#include <algorithm>
#include <cpptoml.h>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <ostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <filesystem>
#include <system_error>
#include <vector>
#include <cstdlib>       // for setenv
#include "includes/Strings.hpp"

struct Action {
    std::string names;
    std::string description;
    std::string command;

    Action(const std::string &nms  = "", 
           const std::string &desc = "", 
           const std::string &cmd  = "") 
        : names(nms), description(desc), command(cmd) {}
};

void from_toml(const cpptoml::table& t, Action& a) {
    // Correct way to access table values
    if (t.contains("names")) {
        a.names = *t.get_as<std::string>("names");
    }
    if (t.contains("description")) {
        a.description = *t.get_as<std::string>("description");
    }
    if (t.contains("command")) {
        a.command = *t.get_as<std::string>("command");
    }
}

std::string shellquote(const std::string& value) {
    std::string quoted = "'";
    for (const char c : value) {
        if (c == '\'') {
            quoted += "'\\''";
        } else {
            quoted += c;
        }
    }
    quoted += "'";
    return quoted;
}

void runnercmd(const std::string cmd, std::string configfile) {
    // fixes a stupid compiler warning
    auto config = cpptoml::parse_file(configfile);
    auto pipe_deleter = [](FILE* fp) { pclose(fp); };
    std::unique_ptr<FILE, decltype(pipe_deleter)> pipe(popen(cmd.c_str(), "r"), pipe_deleter);

    char buffer[256];
    std::string userChoice;

    while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
        userChoice += buffer;
    }

    userChoice.erase(remove_if(userChoice.begin(), userChoice.end(), [](char c) { return c == '\n'; }), userChoice.end());
    if (userChoice.empty()) {
        std::cerr << "No action selected.\n";
        return;
    }

    for (const auto &tableItem : *config) {
        try {
          const auto &table = tableItem.second->as_table();
          if (table->get_as<std::string>("names").value_or("") ==
              userChoice) {

            Action a;
            from_toml(*table, a);
            std::cout << a.description << std::endl;

            int bashResult = system(a.command.c_str());

            if (bashResult != 0) {
              std::cerr << invalidvalue.c_str() << a.command;
              exit(1);
            }
            return;
          }
        } catch (const std::invalid_argument &e) {
          std::cerr << invalidvalue.c_str() << e.what();
          exit(1);
        }
    }

    std::cerr << "No action named " << userChoice << " in the config.\n";
    exit(1);
}

bool setupconfigdir() {
    const char *userHome = getenv("HOME");
    std::string rapidMenuPath = std::string(userHome) + "/.config/RapidMenu";

    if (std::filesystem::exists(rapidMenuPath) && std::filesystem::is_directory(rapidMenuPath)) {
        return false;
    }

    std::error_code ec;
    std::filesystem::create_directories(rapidMenuPath, ec);
    if (ec) {
        std::cerr << "Could not create " << rapidMenuPath << ": " << ec.message() << "\n";
        exit(1);
    }

    std::cout << "Created " << rapidMenuPath << "\n";
    return true;
}

void config(cmd *inputCmd) {
    std::string configfile = inputCmd->config;

    // adds the rapidmenu config if it doesnt exist
    if (setupconfigdir()) {
        exit(1);
    }

    // config handling
    try {
        // 1. Parse config file
        auto config = cpptoml::parse_file(inputCmd->config);
        setenv("LC_CTYPE", "en_US.UTF-8", 1);

        std::string namesList;
        std::vector<std::string> reversedNamesList;

        // Process each item in config
        for (const auto &tableItem : *config) {
            try {
                Action a;
                from_toml(*tableItem.second->as_table(), a);
                reversedNamesList.push_back(a.names);

            } catch (const std::exception &e) {  // Catch all exceptions
                std::cerr << "Error processing action: " << e.what() << "\n";
                exit(1);  // Skip to next item
            }
        }

        //reverses the namelist
        std::reverse(reversedNamesList.begin(), reversedNamesList.end());

        std::ostringstream namesStream;
        bool firstItem = true;  

        for (const auto& n : reversedNamesList) {
            if (!n.empty()) {  
                if (!firstItem) {
                    namesStream << "\n";  
                }
                namesStream << n;
                firstItem = false;
            }
        }

        namesList = namesStream.str();
        std::cout << namesList << "\n";

        std::string rname     = config->get_table("runner") ->get_as<std::string>("rname") .value_or("dashboard:");
        std::string rtheme    = config->get_table("runner") ->get_as<std::string>("rtheme") .value_or("");
        std::string rcommand  = config->get_table("runner") ->get_as<std::string>("rcommand") .value_or("rofi -dmenu -p");

        std::string runnerCommand = "printf '%s' "
            + shellquote(namesList)
            + " | "
            + rcommand
            + " "
            + shellquote(rname + " ")
            + " "
            + rtheme;

        // executes runnerCommand
        std::cout << "Executing config: " << inputCmd->config << "\n";
        
        try {
            runnercmd(runnerCommand, configfile);
                
        } catch (const std::runtime_error& e) {
            std::cerr << "\nError: " << e.what() << "\n";
        }
    } catch (const std::exception &e) {  
        std::cerr << "Config error: " << e.what() << "\n";
    }
}
