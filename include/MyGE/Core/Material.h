//
// Created by Admin on 16/03/2025.
//

#pragma once

#include <map>
#include <string>

namespace My::MyGE {
struct Shader;
struct Texture2D;
class TextureCube;

struct Material {
  const Shader* shader;
  std::map<std::string, const Texture2D*> texture2Ds;
  std::map<std::string, const TextureCube*> textureCubes;
};
}  // namespace My::MyGE

#include "details/Material_AutoRefl.inl"
