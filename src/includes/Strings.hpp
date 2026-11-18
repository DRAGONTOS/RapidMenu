#ifndef STRINGS_H
#define STRINGS_H

#include <string>
#include <string_view>

struct cmd {
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
    -h:           Show this help.
    -c:           To specify which config to use.
    -b:           Make a executable out of a config.

A bare run creates ~/.config/RapidMenu when it is missing.
Configs need a [runner] table; every other table is an action
with names, description and command.
)#";

const std::string invalidvalue = R"#(Invalid value in config: )#";

#endif
