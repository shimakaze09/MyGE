#pragma once

#include "ShaderProperty.h"

#include <map>
#include <string>

namespace My::MyGE {
struct Shader;

struct Material {
  const Shader* shader{nullptr};
  std::map<std::string, ShaderProperty, std::less<>> properties;
};
}  // namespace My::MyGE

#include "details/Material_AutoRefl.inl"
