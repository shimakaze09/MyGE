//
// Created by Admin on 16/03/2025.
//

#pragma once

#include <string>

#include "Object.h"

namespace My::MyGE {
// txt
class TextAsset : public Object {
 public:
  TextAsset(std::string text) : text{std::move(text)} {}
  const std::string& GetText() const noexcept { return text; }

 private:
  std::string text;
};
}  // namespace My::MyGE
