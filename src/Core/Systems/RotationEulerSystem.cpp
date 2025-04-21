#include <MyGE/Core/Components/Rotation.h>
#include <MyGE/Core/Components/RotationEuler.h>
#include <MyGE/Core/Systems/RotationEulerSystem.h>

using namespace Smkz::MyGE;

void RotationEulerSystem::OnUpdate(MyECS::Schedule& schedule) {
  schedule.RegisterEntityJob(
      [](Rotation* rot, const RotationEuler* rot_euler) {
        rot->value = rot_euler->value.to_quat();
      },
      SystemFuncName);
}
