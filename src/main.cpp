#include <iostream>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <cpptoml.h>

using namespace std;
using namespace cpptoml;

const string USAGE = R"#(usage:  RapidMenu [flags] [<command> [args]]
LISTING COMMANDS:
    -c:           To specify which config to use.
    -b:           Make a executable out of a config.
)#";

const string invalidvalue   = R"#(Invalid value in config: )#";
const string invalidconfig  = R"#(Not a valid config: )#";

struct Action {
    string names;
    string description;
    string command;

    Action(
        const string& nms   = "", 
        const string& desc  = "", 
        const string& cmd   = "")
        :
        names(nms), 
        description(desc), 
        command(cmd) {}
};

void from_toml(const table& t, Action& a) {
    try {
        a.names         = *t.get_as<string>("names");
        a.description   = *t.get_as<string>("description");
        a.command       = *t.get_as<string>("command");
    } catch (const parse_exception& e) {
        throw invalid_argument(invalidvalue.c_str());
    }
}

int main(int argc, char* argv[]) {
    if (argc > 1 && strcmp(argv[1], "-c") == 0) {
        if (argc < 3 || argv[2][0] == '-') {
            cerr << USAGE.c_str() << endl;
            return 1;
        }

        const char* configFile  = nullptr; 
        const char* userHome    = getenv("HOME");
        string rapidMenuPath    = string(userHome) + "/.config/RapidMenu";
        string rapidcommand     = "mkdir -p " + rapidMenuPath;

        configFile              = argv[2];

        if (filesystem::exists(rapidMenuPath) && filesystem::is_directory(rapidMenuPath)) {
        } else {
            system(rapidcommand.c_str());
            cerr << "Setting up config." << endl;
            return 1;
        }

        try {
            auto config = parse_file(configFile);

            setenv("LC_CTYPE", "en_US.UTF-8", 1);
            string namesList;
            vector<string> reversedNamesList;

            for (const auto& tableItem : *config) {
                try {
                    Action a;
                    from_toml(*tableItem.second->as_table(), a);

                    reversedNamesList.push_back(a.names);

                } catch (const invalid_argument& e) {
                    cerr << invalidvalue.c_str() << e.what() << endl;
                    return 1;
                }
            }

            reverse(reversedNamesList.begin(), reversedNamesList.end());
            ostringstream namesStream;
            for (const auto& name : reversedNamesList) {
                if (!namesStream.str().empty()) {
                    namesStream << "\n";
                }
                namesStream << name;
            }

            namesList = namesStream.str();

            string rname    = config->get_table("runner")->get_as<string>("rname").value_or("dashboard:");
            string rtheme   = config->get_table("runner")->get_as<string>("rtheme").value_or("");
            string rcommand = config->get_table("runner")->get_as<string>("rcommand").value_or("rofi -dmenu -p");

            string rofiCommand  = "printf '" + namesList + "' | " + rcommand + " '" + rname + " ' " + rtheme;
            FILE *rofiProcess   = popen(rofiCommand.c_str(), "r");

            char buffer[256];
            string userChoice;

            while (fgets(buffer, sizeof(buffer), rofiProcess)) {
                userChoice += buffer;
            }

            userChoice.erase(remove_if(userChoice.begin(), userChoice.end(), [](char c) { return c == '\n'; }), userChoice.end());
            for (const auto& tableItem : *config) {
                try {
                    const auto& table = tableItem.second->as_table();
                    if (table->get_as<string>("names").value_or("") == userChoice) {

                        Action a;
                        from_toml(*table, a);
                        int bashResult = system(a.command.c_str());
                        cout << a.description << endl;

                        if (bashResult != 0) {
                            cerr << invalidvalue.c_str() << a.command << endl;
                        }
                    }
                } catch (const invalid_argument& e) {
                    cerr << invalidvalue.c_str() << e.what() << endl;
                    return 1;
                }
            }

        } catch (const parse_exception& e) {
            cerr << "Incorrect config file: " << endl;
            return 1;
        }

    // executable 
    } else if (argc > 1 && strcmp(argv[1], "-b") == 0) {
        if (argc < 3 || argv[2][0] == '-') {
            cerr << USAGE.c_str() << endl;
            return 1;
        }

        const char* bconfig     = nullptr;
        const char* bexe        = nullptr;
        const char* userHome    = getenv("HOME");

        string bexeout;
        string byn;
        string bconfigin        = argv[2];
        string bindir           = string(userHome) + "/.local/bin";
        string createbindir     = "mkdir -p " + bindir;

        if (filesystem::exists(bindir) && filesystem::is_directory(bindir)) {
        } else {
            system(createbindir.c_str());
            cerr << "Setting up bin dir." << endl;
            return 1;
        }

        // config
        string bconfigfile = bconfigin; 
        if (filesystem::exists(bconfigfile) && filesystem::is_regular_file(bconfigfile)) {
            bconfig = argv[2];

        } else {
            while (!(filesystem::exists(bconfigfile) && filesystem::is_regular_file(bconfigfile))) {
                cout << "Invalid config file: " << bconfigfile << endl;
                cout << "Please enter a valid config: ";
                cin >> bconfigfile;

                bconfig = bconfigfile.c_str();
            }

        }

        // executable
        cout << "What do you want to call your executable?: ";
        cin >> bexeout;
        string bexefile = string(userHome) + "/.local/bin/" + bexeout; 

        if (filesystem::exists(bexefile) && filesystem::is_regular_file(bexefile)) {
            while (filesystem::exists(bexefile) && filesystem::is_regular_file(bexefile)) {
                cout << "do you want to overwrite: " << bexeout << "? (y/n) ";
                cin >> byn;

                transform(byn.begin(), byn.end(), byn.begin(), ::tolower);

                if (byn == "y") {
                    bexe = bexeout.c_str();
                    break;
                } else if (byn == "n") {
                    cout << "What do you want to call your executable?: ";
                    cin >> bexeout;
                    bexe = bexeout.c_str();
                }
            }

        } else {
            bexe = bexeout.c_str();
        }

        system(("touch " + bexefile + string(bexe)).c_str());
        system(("chmod +x " + bexefile + string(bexe)).c_str());
        system(("echo '#!/usr/bin/env bash' > " + bexefile + string(bexe)).c_str());
        system(("echo 'RapidMenu -c " + string(bconfig) + "' >> " + string(bexe)).c_str());

        }else{
            cerr << USAGE.c_str() << endl;
            return 1;
    }
    return 0;
}
