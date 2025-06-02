#pragma once
namespace Smkz::MyECS {
class World;
}

namespace Smkz::MyGE {
void MyDRefl_Register_Render();
void World_Register_Render(MyECS::World* world);
}  // namespace Smkz::MyGE
