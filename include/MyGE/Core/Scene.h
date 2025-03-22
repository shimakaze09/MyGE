#pragma once

#include "Object.h"

#include <string>

namespace My::MyGE {
class Scene : public Object {
 public:
  Scene(std::string text) : text{std::move(text)} {}

  const std::string& GetText() const noexcept { return text; }

 private:
  std::string text;
};
}  // namespace My::MyGE
