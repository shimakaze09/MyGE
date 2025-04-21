#include <MyGE/Core/Components/LocalToWorld.h>
#include <MyGE/Core/Components/WorldToLocal.h>
#include <MyGE/Core/Systems/WorldToLocalSystem.h>

using namespace Smkz::MyGE;

void WorldToLocalSystem::OnUpdate(MyECS::Schedule& schedule) {
  schedule.RegisterEntityJob(
      [](WorldToLocal* w2l, const LocalToWorld* l2w) {
        w2l->value = l2w->value.inverse();
      },
      SystemFuncName);
}
