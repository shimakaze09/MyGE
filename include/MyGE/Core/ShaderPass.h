//
// Created by Admin on 21/03/2025.
//

#pragma once

#include <string>

namespace My::MyGE {
struct ShaderPass {
  std::string vertexName;    // e.g. vert
  std::string fragmentName;  // e.g. frag
};
}  // namespace My::MyGE

#include "details/ShaderPass_AutoRefl.inl"
