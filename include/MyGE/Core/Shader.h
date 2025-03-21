//
// Created by Admin on 15/03/2025.
//

#pragma once

#include "Object.h"
#include "ShaderPass.h"

#include <string>
#include <vector>

namespace My::MyGE {
class HLSLFile;

struct Shader : Object {
  const HLSLFile* hlslFile{nullptr};
  std::string shaderName;  // e.g. a/b/c/d
  std::string targetName;  // e.g. 5_0
  std::vector<ShaderPass> passes;
};
}  // namespace My::MyGE

#include "details/Shader_AutoRefl.inl"
