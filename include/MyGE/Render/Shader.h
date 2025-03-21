#pragma once

#include <string>
#include <vector>

#include "RenderRsrcObject.h"
#include "RootParameter.h"
#include "ShaderPass.h"

namespace My::MyGE {
class HLSLFile;

struct Shader : RenderRsrcObject {
  const HLSLFile* hlslFile{nullptr};
  std::string shaderName;  // e.g. a/b/c/d
  std::string targetName;  // e.g. 5_0
  std::vector<RootParameter> rootParameters;
  std::vector<ShaderPass> passes;
};
}  // namespace My::MyGE

#include "details/Shader_AutoRefl.inl"
