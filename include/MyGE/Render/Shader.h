#pragma once

#include "RenderRsrcObject.h"
#include "RootParameter.h"
#include "ShaderPass.h"
#include "ShaderProperty.h"

#include <map>
#include <string>
#include <vector>

namespace My::MyGE {
class HLSLFile;

struct Shader : RenderRsrcObject {
  const HLSLFile* hlslFile{nullptr};
  std::string shaderName;  // e.g. a/b/c/d
  std::string targetName;  // e.g. 5_0
  std::vector<RootParameter> rootParameters;
  std::map<std::string, ShaderProperty, std::less<>> properties;
  std::vector<ShaderPass> passes;
};
}  // namespace My::MyGE

#include "details/Shader_AutoRefl.inl"
