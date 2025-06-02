#include <MyGE/Core/Components/Components.h>
#include <MyGE/Core/Register_Core.h>
#include <MyGE/Core/Systems/Systems.h>

using namespace Smkz::MyGE;
using namespace Smkz::MyECS;

void Smkz::MyGE::World_Register_Core(World* w) {
  w->entityMngr.cmptTraits
      .Register<Children, Input, LocalToParent, LocalToWorld, Name, Parent,
                Roamer, Rotation, RotationEuler, Scale, NonUniformScale,
                Translation, WorldTime, WorldToLocal>();

  w->systemMngr.systemTraits
      .Register<InputSystem, LocalToParentSystem, RoamerSystem,
                RotationEulerSystem, TRSToLocalToParentSystem,
                TRSToLocalToWorldSystem, WorldToLocalSystem, WorldTimeSystem>();
}
