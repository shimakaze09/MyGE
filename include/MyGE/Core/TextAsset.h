#pragma once

#include "Object.h"

#include <string>

namespace My::MyGE {
// txt, json
class TextAsset : public Object {
 public:
  TextAsset(std::string text) : text{std::move(text)} {}

  const std::string& GetText() const noexcept { return text; }

 private:
  std::string text;
};
}  // namespace My::MyGE
