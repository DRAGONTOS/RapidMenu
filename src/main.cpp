#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <cpptoml.h>

using namespace std;
using namespace cpptoml;

struct Action {
    string names;
    string description;
    string command;

    Action(const string& nms = "", const string& desc = "", const string& cmd = "")
        : names(nms), description(desc), command(cmd) {}
};

void from_toml(const table& t, Action& a) {
    try {
        a.names = *t.get_as<string>("names");
        a.description = *t.get_as<string>("description");
        a.command = *t.get_as<string>("command");
    } catch (const parse_exception& e) {
        throw std::invalid_argument("Error getting value from config:");
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3 || argv[2][0] == '-') {
    cerr << "Usage: -c <config_file>" << endl;
    return 1;
    }

    const string configFile2 = argv[2];

    try {
        auto config = parse_file(configFile2);

        // Create the Rofi command
        setenv("LC_CTYPE", "", 1);  // Unset LC_CTYPE
        string namesList;

        for (const auto& tableItem : *config) {
            try {
                // Use the `from_toml` function to convert TOML to Action
                Action a;
                from_toml(*tableItem.second->as_table(), a);
                namesList += a.names + "\n";
            } catch (const std::invalid_argument& e) {
                cerr << "Error getting value from config: " << e.what() << endl;
                return 1;
            }
        }

        string rname = config->get_table("runner")->get_as<string>("rname").value_or("");
        string rtheme = config->get_table("runner")->get_as<string>("rtheme").value_or("");
        string rcommand = config->get_table("runner")->get_as<string>("rcommand").value_or("");
        //string rofiCommand = "printf '" + namesList + "' | rofi -dmenu -p 'Dashboard: ' -show-icons -theme ~/.config/rofi/themes/rounded-purple-dark.rasi";
        string rofiCommand = "printf '" + namesList + "' | " + rcommand + " '" + rname + " ' " + rtheme;
        // Open a pipe to the Rofi process
        FILE *rofiProcess = popen(rofiCommand.c_str(), "r");

        char buffer[256];
        string userChoice;

        // Read user input from Rofi using getline
        while (fgets(buffer, sizeof(buffer), rofiProcess)) {
            userChoice += buffer;
        }

        // Remove newline characters from userChoice
        userChoice.erase(remove_if(userChoice.begin(), userChoice.end(), [](char c) { return c == '\n'; }), userChoice.end());

        for (const auto& tableItem : *config) {
            try {
                const auto& table = tableItem.second->as_table();
                if (table->get_as<string>("names").value_or("") == userChoice) {
                    // Use the `from_toml` function to convert TOML to Action
                    Action a;
                    from_toml(*table, a);
                    int bashResult = system(a.command.c_str());
                    cout << a.description << endl;

                    // Check the return value if needed
                    if (bashResult != 0) {
                        cerr << "Error executing command: " << a.command << endl;
                    }

                    return 0;
                }
            } catch (const std::invalid_argument& e) {
                cerr << "Error getting value from config: " << e.what() << endl;
                return 1;
            }
        }

        cout << "Invalid choice. Please enter a valid option." << endl;

    } catch (const parse_exception& e) {
        cerr << "Incorrect config file: " << endl;
        return 1;
    }

    return 0;
}
