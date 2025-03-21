#pragma once

#include <map>
#include <string>

namespace My::MyGE {
struct ShaderPass {
  std::string vertexName;    // e.g. vert
  std::string fragmentName;  // e.g. frag
  std::map<std::string, std::string, std::less<>> tags;
};
}  // namespace My::MyGE

#include "details/ShaderPass_AutoRefl.inl"
