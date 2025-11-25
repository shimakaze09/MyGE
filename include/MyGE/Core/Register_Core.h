#pragma once

namespace My::MyECS {
class World;
}

namespace My::MyGE {
void MyDRefl_Register_Core();
void World_Register_Core(MyECS::World* w);
}  // namespace My::MyGE

