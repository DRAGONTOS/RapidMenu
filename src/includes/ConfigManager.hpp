#ifndef CONFIGMANAGER_H
#define  CONFIGMANAGER_H
#include <cpptoml.h>

struct Action {
  std::string names;
  std::string description;
  std::string command;

  Action(const std::string &nms = "", const std::string &desc = "", const std::string &cmd = "") : names(nms), description(desc), command(cmd) {}
};

void from_toml(const cpptoml::table &t, Action &a);
#endif
