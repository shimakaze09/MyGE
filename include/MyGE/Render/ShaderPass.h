#pragma once

#include "RenderState.h"

#include <map>
#include <string>

namespace My::MyGE {
struct ShaderPass {
  std::string vertexName; 
  std::string fragmentName;
  RenderState renderState;
  std::map<std::string, std::string, std::less<>> tags;

  // Background 1000
  // Geometry (default) 2000
  // AlphaTest 2450
  // Transparent 2500
  // Overlay 4000
  //  < 2500 : "opaque", optimize the drawing order
  // >= 2500 : "transparent", in back-to-front order
  enum class Queue : size_t {
    Background = 1000,
    Geometry = 2000,
    AlphaTest = 2450,
    Transparent = 2500,
    Overlay = 4000
  };
  size_t queue{2000};
};
}  // namespace My::MyGE

#include "details/ShaderPass_AutoRefl.inl"
