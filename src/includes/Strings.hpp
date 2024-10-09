#pragma once
#include <string>
#include <string_view>

const std::string_view USAGE = R"#(usage:  RapidMenu [flags] [<command> [args]]
LISTING COMMANDS:
    -c:           To specify which config to use.
    -b:           Make a executable out of a config.
)#";

const std::string invalidvalue = R"#(Invalid value in config: )#";
const std::string invalidconfig = R"#(Not a valid config: )#";


