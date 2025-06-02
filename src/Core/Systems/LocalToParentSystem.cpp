#include <MyGE/Core/Components/Children.h>
#include <MyGE/Core/Components/LocalToParent.h>
#include <MyGE/Core/Components/LocalToWorld.h>
#include <MyGE/Core/Components/Parent.h>
#include <MyGE/Core/Systems/LocalToParentSystem.h>
#include <MyGE/Core/Systems/TRSToLocalToParentSystem.h>
#include <MyGE/Core/Systems/TRSToLocalToWorldSystem.h>

using namespace Smkz::MyGE;
using namespace Smkz::MyECS;

void LocalToParentSystem::ChildLocalDeserializeToWorld(
    World* w, const transformf& parent_l2w, Entity e) {
  transformf l2w;
  if (w->entityMngr.Have(e, TypeID_of<LocalToWorld>) &&
      w->entityMngr.Have(e, TypeID_of<LocalToParent>)) {
    auto child_l2w = w->entityMngr.WriteComponent<LocalToWorld>(e);
    auto child_l2p = w->entityMngr.ReadComponent<LocalToParent>(e);
    l2w = parent_l2w * child_l2p->value;
    child_l2w->value = l2w;
  } else
    l2w = parent_l2w;

  if (w->entityMngr.Have(e, TypeID_of<Children>)) {
    auto children = w->entityMngr.ReadComponent<Children>(e);
    for (const auto& child : children->value)
      ChildLocalDeserializeToWorld(w, l2w, child);
  }
}

void LocalToParentSystem::OnUpdate(Schedule& schedule) {
  schedule.AddNone(TRSToLocalToWorldSystem::SystemFuncName, TypeID_of<Parent>);
  schedule.RegisterEntityJob(
      [](World* w, const LocalToWorld* l2w, const Children* children) {
        for (const auto& child : children->value)
          ChildLocalDeserializeToWorld(w, l2w->value, child);
      },
      SystemFuncName,
      Schedule::EntityJobConfig{
          .archetypeFilter = {.none = {TypeID_of<Parent>}},
          .randomAccessor = {
              .types = {MyECS::AccessTypeID_of<Write<LocalToWorld>>,
                        MyECS::AccessTypeID_of<Latest<Children>>,
                        MyECS::AccessTypeID_of<Latest<LocalToParent>>},
          }});
  schedule.Order(TRSToLocalToWorldSystem::SystemFuncName,
                 LocalToParentSystem::SystemFuncName);
}
