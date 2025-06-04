#ifndef STRING_H
#define STRING_H

#include <string>
#include <string_view>
#include <vector>

struct cmd {
const char* userHome  = getenv("HOME");
// what the args need 
std::string config;
std::string exename;
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
