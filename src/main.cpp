#include <algorithm>
#include <cctype>
#include <cpptoml.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

const std::string USAGE = R"#(usage:  RapidMenu [flags] [<command> [args]]
LISTING COMMANDS:
    -c:           To specify which config to use.
    -b:           Make a executable out of a config.
)#";

const std::string invalidvalue  = R"#(Invalid value in config: )#";
const std::string invalidconfig = R"#(Not a valid config: )#";

struct Action {
  std::string names;
  std::string description;
  std::string command;

  Action(const std::string &nms = "", const std::string &desc = "",
         const std::string &cmd = "")
      : names(nms), description(desc), command(cmd) {}
};

void from_toml(const cpptoml::table &t, Action &a) {
  try {
    a.names       = *t.get_as<std::string>("names");
    a.description = *t.get_as<std::string>("description");
    a.command     = *t.get_as<std::string>("command");
  } catch (const cpptoml::parse_exception &e) {
    throw std::invalid_argument(invalidvalue.c_str());
  }
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args(argv, argv + argc);

  const std::string userHome = std::getenv("HOME");

  std::string rapidMenuPath = std::string(userHome) + "/.config/RapidMenu";
  std::string rapidcommand  = "mkdir -p " + rapidMenuPath;

  if (std::filesystem::exists(rapidMenuPath) &&
      std::filesystem::is_directory(rapidMenuPath)) {
  } else {
    system(rapidcommand.c_str());
    std::cerr << "Setting up the config directory: " << rapidMenuPath;
    return 1;
  }

  if (argc > 1 && strcmp(argv[1], "-c") == 0) {
    if (argc < 3 || argv[2][0] == '-') {
      std::cerr << USAGE.c_str();
      return 1;
    }

    const char *configFile = nullptr;

    configFile = argv[2];

    try {
      auto config = cpptoml::parse_file(configFile);

      setenv("LC_CTYPE", "en_US.UTF-8", 1);
      std::string namesList;
      std::vector<std::string> reversedNamesList;

      for (const auto &tableItem : *config) {
        try {
          Action a;
          from_toml(*tableItem.second->as_table(), a);

          reversedNamesList.push_back(a.names);

        } catch (const std::invalid_argument &e) {
          std::cerr << invalidvalue.c_str() << e.what();
          return 1;
        }
      }

      reverse(reversedNamesList.begin(), reversedNamesList.end());
      std::ostringstream namesStream;
      for (const auto &name : reversedNamesList) {
        if (!namesStream.str().empty()) {
          namesStream << "\n";
        }
        namesStream << name;
      }

      namesList = namesStream.str();

      std::string rname  =
          config->get_table("runner")->get_as<std::string>("rname").value_or(
              "dashboard:");
      std::string rtheme =
          config->get_table("runner")->get_as<std::string>("rtheme").value_or(
              "");
      std::string rcommand    = config->get_table("runner")
                                 ->get_as<std::string>("rcommand")
                                 .value_or("rofi -dmenu -p");

      std::string rofiCommand = "printf '" + namesList + "' | " + rcommand +
                                " '" + rname + " ' " + rtheme;
      std::FILE *rofiProcess  = popen(rofiCommand.c_str(), "r");

      char buffer[256];
      std::string userChoice;

      while (fgets(buffer, sizeof(buffer), rofiProcess)) {
        userChoice += buffer;
      }

      userChoice.erase(remove_if(userChoice.begin(), userChoice.end(),
                                 [](char c) { return c == '\n'; }),
                       userChoice.end());
      for (const auto &tableItem : *config) {
        try {
          const auto &table = tableItem.second->as_table();
          if (table->get_as<std::string>("names").value_or("") == userChoice) {

            Action a;
            from_toml(*table, a);
            int bashResult = system(a.command.c_str());
            std::cout << a.description;

            if (bashResult != 0) {
              std::cerr << invalidvalue.c_str() << a.command;
            }
          }
        } catch (const std::invalid_argument &e) {
          std::cerr << invalidvalue.c_str() << e.what();
          return 1;
        }
      }

    } catch (const cpptoml::parse_exception &e) {
      std::cerr << "Incorrect config file: ";
      return 1;
    }

    // executable
  } else if (argc > 1 && strcmp(argv[1], "-b") == 0) {
    if (argc < 3 || argv[2][0] == '-') {
      std::cerr << USAGE.c_str();
      return 1;
    }

    const char *bconfig        = nullptr;
    const char *bexe           = nullptr;
    const std::string userHome = std::getenv("HOME");

    std::string bexeout;
    std::string byn;
    std::string bconfigin    = argv[2];
    std::string bindir       = std::string(userHome) + "/.local/bin";
    std::string createbindir = "mkdir -p " + bindir;

    if (std::filesystem::exists(bindir) &&
        std::filesystem::is_directory(bindir)) {
    } else {
      system(createbindir.c_str());
      std::cerr << "Setting up bin dir.";
      return 1;
    }

    // config
    std::string bconfigfile = bconfigin;
    if (std::filesystem::exists(bconfigfile) &&
        std::filesystem::is_regular_file(bconfigfile)) {
      bconfig = argv[2];

    } else {
      while (!(std::filesystem::exists(bconfigfile) &&
               std::filesystem::is_regular_file(bconfigfile))) {
        std::cout << "Invalid config file: " << bconfigfile;
        std::cout << "Please enter a valid config: ";
        std::cin >> bconfigfile;

        bconfig = bconfigfile.c_str();
      }
    }

    // executable
    std::cout << "What do you want to call your executable?: ";
    std::cin >> bexeout;
    std::string bexefile = std::string(userHome) + "/.local/bin/" + bexeout;

    if (std::filesystem::exists(bexefile) &&
        std::filesystem::is_regular_file(bexefile)) {
      while (std::filesystem::exists(bexefile) &&
             std::filesystem::is_regular_file(bexefile)) {
        std::cout << "do you want to overwrite: " << bexeout << "? (y/n) ";
        std::cin >> byn;

        transform(byn.begin(), byn.end(), byn.begin(), ::tolower);

        if (byn == "y") {
          bexe = bexeout.c_str();
          break;
        } else if (byn == "n") {
          std::cout << "What do you want to call your executable?: ";
          std::cin >> bexeout;
          bexe = bexeout.c_str();
        }
      }

    } else {
      bexe = bexeout.c_str();
    }

    system(("touch " + bexefile + std::string(bexe)).c_str());
    system(("chmod +x " + bexefile + std::string(bexe)).c_str());
    system(("echo '#!/usr/bin/env bash' > " + bexefile + std::string(bexe))
               .c_str());
    system(("echo 'RapidMenu -c " + std::string(bconfig) + "' >> " +
            std::string(bexe))
               .c_str());

  } else {
    std::cerr << USAGE.c_str();
    return 1;
  }
  return 0;
}
