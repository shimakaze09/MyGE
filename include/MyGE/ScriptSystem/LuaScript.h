#pragma once

#include "../Core/Object.h"

#include <string>

namespace My::MyGE {
class LuaScript : public Object {
 public:
  LuaScript(std::string str) : str(std::move(str)) {}

  const std::string& GetText() const noexcept { return str; }

 private:
  std::string str;
};
}  // namespace My::MyGE
