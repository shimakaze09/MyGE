//
// Created by Admin on 16/03/2025.
//

#include <MyGE/Core/Components/Camera.h>
#include <MyGE/Core/Systems/CameraSystem.h>

using namespace My::MyGE;
using namespace My::MyECS;

void CameraSystem::OnUpdate(Schedule& schedule) {
  schedule.RegisterEntityJob(
      [](Camera* camera) {
        camera->prjectionMatrix = transformf::perspective(
            to_radian(camera->fov), camera->aspect, camera->clippingPlaneMin,
            camera->clippingPlaneMax, 0.f);
      },
      SystemFuncName);
}