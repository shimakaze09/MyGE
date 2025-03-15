//
// Created by Admin on 15/03/2025.
//

#pragma once

#include <string>

namespace My::MyGE {
class LuaScript {
 public:
  LuaScript(std::string str) : str(std::move(str)) {}
  const std::string& GetString() const noexcept { return str; }

 private:
  std::string str;
};
}  // namespace My::MyGE
