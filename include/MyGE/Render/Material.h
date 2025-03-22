#pragma once

#include "../Core/Object.h"
#include "ShaderProperty.h"

#include <map>
#include <string>

namespace My::MyGE {
struct Shader;

struct Material : Object {
  std::shared_ptr<const Shader> shader;
  std::map<std::string, ShaderProperty, std::less<>> properties;
};
}  // namespace My::MyGE

#include "details/Material_AutoRefl.inl"
