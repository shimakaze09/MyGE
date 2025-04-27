#pragma once

#include <map>
#include <memory>
#include <string>

#include "../Core/SharedVar.h"
#include "GPURsrc.h"
#include "ShaderProperty.h"

namespace Smkz::MyGE {
struct Shader;

struct Material : GPURsrc {
  SharedVar<Shader> shader;
  std::map<std::string, ShaderProperty, std::less<>> properties;
};
}  // namespace Smkz::MyGE
