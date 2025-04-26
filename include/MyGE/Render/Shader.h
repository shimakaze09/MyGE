#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "GPURsrc.h"
#include "RootParameter.h"
#include "ShaderPass.h"
#include "ShaderProperty.h"

namespace Smkz::MyGE {
class HLSLFile;

struct Shader : GPURsrc {
  // virtual ~Shader();

  SharedVar<HLSLFile> hlslFile;
  std::string name;  // e.g. a/b/c/d
  std::vector<RootParameter> rootParameters;
  std::map<std::string, ShaderProperty, std::less<>> properties;
  std::vector<ShaderPass> passes;
};
}  // namespace Smkz::MyGE
