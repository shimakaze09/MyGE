//
// Created by Admin on 17/03/2025.
//

#include "InitTransform.h"

#include <MyGE/Transform/Components/Children.h>
#include <MyGE/Transform/Components/LocalToParent.h>
#include <MyGE/Transform/Components/LocalToWorld.h>
#include <MyGE/Transform/Components/Parent.h>
#include <MyGE/Transform/Components/Rotation.h>
#include <MyGE/Transform/Components/RotationEuler.h>
#include <MyGE/Transform/Components/Scale.h>
#include <MyGE/Transform/Components/Translation.h>
#include <MyGE/Transform/Components/WorldToLocal.h>

void My::MyGE::detail::InitTransform(lua_State* L) {
  MyLuaPP::Register<Children>(L);
  MyLuaPP::Register<LocalToParent>(L);
  MyLuaPP::Register<LocalToWorld>(L);
  MyLuaPP::Register<Parent>(L);
  MyLuaPP::Register<Rotation>(L);
  MyLuaPP::Register<RotationEuler>(L);
  MyLuaPP::Register<Scale>(L);
  MyLuaPP::Register<Translation>(L);
  MyLuaPP::Register<WorldToLocal>(L);
}
