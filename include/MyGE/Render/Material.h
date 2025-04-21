#pragma once

#include <map>
#include <memory>
#include <string>

#include "GPURsrc.h"
#include "ShaderProperty.h"

namespace Smkz::MyGE {
struct Shader;

struct Material : GPURsrc {
  std::shared_ptr<const Shader> shader;
  std::map<std::string, ShaderProperty, std::less<>> properties;
};
}  // namespace Smkz::MyGE
