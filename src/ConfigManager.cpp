#include <cctype>
#include <cpptoml.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>
#include "includes/ConfigManager.hpp"
#include "includes/Strings.hpp"

void from_toml(const cpptoml::table &t, Action &a) {
  try {
    a.names       = *t.get_as<std::string>("names");
    a.description = *t.get_as<std::string>("description");
    a.command     = *t.get_as<std::string>("command");
  } catch (const cpptoml::parse_exception &e) {
    throw std::invalid_argument(invalidvalue.c_str());
  }
}
