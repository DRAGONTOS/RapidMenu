#ifndef STRING_H
#define STRING_H

#include <string>
#include <string_view>
#include <vector>

struct cmd {
const char* userHome  = getenv("HOME");
// buffers
std::string ids;
std::string source;

// what the args need 
std::string config;
std::string exename;

// total mods
int totalmods = 0;

// Modnamestuff
std::vector<std::string> sucids;

int successes = 0;
int timedout  = 0;
int errors    = 0;
int totalmeow = 0;

size_t threadsCompleted = 0;
};

const std::string_view USAGE = R"#(usage:  RapidMenu [flags] [<command> [args]]
LISTING COMMANDS:
    -c:           To specify which config to use.
    -b:           Make a executable out of a config.
)#";

const std::string_view HELP = R"#(usage:  RapidMenu [flags] [<command> [args]]
LISTING COMMANDS:
    -c:           To specify which config to use.
    -b:           Make a executable out of a config.
)#";

const std::string invalidvalue = R"#(Invalid value in config: )#";
const std::string invalidconfig = R"#(Not a valid config: )#";

#endif
