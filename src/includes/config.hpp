#ifndef CONFIG_H
#define CONFIG_H
#include "Strings.hpp"

#include <string>

std::string shellquote(const std::string& value);
bool setupconfigdir();
void config(cmd *inputCmd);

#endif // !CONFIG_H
#define CONFIG_H

