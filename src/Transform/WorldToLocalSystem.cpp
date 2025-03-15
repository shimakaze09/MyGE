//
// Created by Admin on 13/03/2025.
//

#include <MyGE/Transform/Components/LocalToWorld.h>
#include <MyGE/Transform/Components/WorldToLocal.h>
#include <MyGE/Transform/Systems/WorldToLocalSystem.h>

using namespace My::MyGE;

void WorldToLocalSystem::OnUpdate(MyECS::Schedule& schedule) {
  schedule.RegisterEntityJob(
      [](WorldToLocal* w2l, const LocalToWorld* l2w) {
        w2l->value = l2w->value.inverse();
      },
      SystemFuncName);
}
