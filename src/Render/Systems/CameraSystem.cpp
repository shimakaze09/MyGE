#include <MyGE/Render/Components/Camera.h>
#include <MyGE/Render/Systems/CameraSystem.h>

using namespace Smkz::MyGE;
using namespace Smkz::MyECS;

void CameraSystem::OnUpdate(Schedule& schedule) {
  schedule.RegisterEntityJob(
      [](Camera* camera) {
        camera->prjectionMatrix = transformf::perspective(
            to_radian(camera->fov), camera->aspect, camera->clippingPlaneMin,
            camera->clippingPlaneMax, 0.f);
      },
      SystemFuncName);
}
