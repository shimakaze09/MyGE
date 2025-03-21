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
  std::string name;  // e.g. a/b/c/d
  std::vector<RootParameter> rootParameters;
  std::map<std::string, ShaderProperty, std::less<>> properties;
  std::vector<ShaderPass> passes;
};
}  // namespace My::MyGE

#include "details/Shader_AutoRefl.inl"
