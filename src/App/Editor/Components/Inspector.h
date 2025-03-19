//
// Created by Admin on 19/03/2025.
//

#include <MyECS/Entity.h>

namespace My::MyGE {
struct Inspector {
  bool lock{false};
  MyECS::Entity target;
};
}  // namespace My::MyGE
