//
// Created by Admin on 13/03/2025.
//

#include <MyGE/Transform/Components/Rotation.h>
#include <MyGE/Transform/Components/RotationEuler.h>
#include <MyGE/Transform/Systems/RotationEulerSystem.h>

using namespace My::MyGE;

void RotationEulerSystem::OnUpdate(MyECS::Schedule& schedule) {
  schedule.RegisterEntityJob(
      [](Rotation* rot, const RotationEuler* rot_euler) {
        rot->value = rot_euler->value.to_quat();
      },
      SystemFuncName);
}
