#pragma once

#include <MyECS/World.h>

namespace My::MyGE {
struct SystemController {
  MyECS::World* world{nullptr};
};
}  // namespace My::MyGE
