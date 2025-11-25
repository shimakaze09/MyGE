#pragma once
namespace My::MyECS {
class World;
}

namespace My::MyGE {
void MyDRefl_Register_Render();
void World_Register_Render(MyECS::World* world);
}  // namespace My::MyGE

