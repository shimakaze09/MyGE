#pragma once

namespace Smkz::MyECS {
class World;
}

namespace Smkz::MyGE {
void MyDRefl_Register_Core();
void World_Register_Core(MyECS::World* w);
}  // namespace Smkz::MyGE
