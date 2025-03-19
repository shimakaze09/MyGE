//
// Created by Admin on 19/03/2025.
//

#include "HierarchySystem.h"

#include "../Components/Hierarchy.h"

#include <MyGE/Core/Components/Name.h>
#include <MyGE/Transform/Components/Components.h>

#include <MyGE/_deps/imgui/imgui.h>

using namespace My::MyGE;

namespace My::MyGE::detail {
void HierarchyPrintEntity(Hierarchy* hierarchy, MyECS::Entity e) {
  static constexpr ImGuiTreeNodeFlags nodeBaseFlags =
      ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
      ImGuiTreeNodeFlags_SpanAvailWidth;

  ImGuiTreeNodeFlags nodeFlags = nodeBaseFlags;
  if (hierarchy->select == e)
    nodeFlags |= ImGuiTreeNodeFlags_Selected;

  auto name = hierarchy->world->entityMngr.Get<Name>(e);
  auto children = hierarchy->world->entityMngr.Get<Children>(e);

  if (children) {
    bool nodeOpen =
        name ? ImGui::TreeNodeEx((void*)(intptr_t)e.Idx(), nodeFlags, "%s (%d)",
                                 name->value.c_str(), e.Idx())
             : ImGui::TreeNodeEx((void*)(intptr_t)e.Idx(), nodeFlags,
                                 "Entity (%d)", e.Idx());
    if (ImGui::IsItemClicked())
      hierarchy->select = e;
    if (nodeOpen) {
      for (const auto& child : children->value)
        HierarchyPrintEntity(hierarchy, child);
      ImGui::TreePop();
    }
  } else {
    nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    if (name)
      ImGui::TreeNodeEx((void*)(intptr_t)e.Idx(), nodeFlags, "%s (%d)",
                        name->value.c_str(), e.Idx());
    else
      ImGui::TreeNodeEx((void*)(intptr_t)e.Idx(), nodeFlags, "Entity (%d)",
                        e.Idx());
    if (ImGui::IsItemClicked())
      hierarchy->select = e;
  }
}
}  // namespace My::MyGE::detail

void HierarchySystem::OnUpdate(MyECS::Schedule& schedule) {
  schedule.RegisterJob(
      [](MyECS::World* w,
         MyECS::Latest<MyECS::Singleton<Hierarchy>> hierarchy) {
        w->AddCommand([hierarchy](MyECS::World* w) {
          if (ImGui::Begin("Hierarchy")) {

            MyECS::ArchetypeFilter filter;
            filter.none = {MyECS::CmptType::Of<Parent>};
            const_cast<MyECS::World*>(hierarchy->world)
                ->RunEntityJob(
                    [=](MyECS::Entity e) {
                      detail::HierarchyPrintEntity(
                          const_cast<Hierarchy*>(hierarchy.Get()), e);
                    },
                    false, filter);
          }
          ImGui::End();
        });
      },
      "HierarchySystem");
}
