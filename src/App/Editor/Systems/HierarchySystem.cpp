//
// Created by Admin on 19/03/2025.
//

#include "HierarchySystem.h"

#include "../Components/Hierarchy.h"

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

  auto children = hierarchy->world->entityMngr.Get<Children>(e);

  if (children) {
    bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)e.Idx(), nodeFlags,
                                      "Entity %d", e.Idx());
    if (ImGui::IsItemClicked())
      hierarchy->select = e;
    if (nodeOpen) {
      for (const auto& child : children->value)
        HierarchyPrintEntity(hierarchy, child);
      ImGui::TreePop();
    }
  } else {
    nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    ImGui::TreeNodeEx((void*)(intptr_t)e.Idx(), nodeFlags, "Entity %d",
                      e.Idx());
    if (ImGui::IsItemClicked())
      hierarchy->select = e;
  }
}
}  // namespace My::MyGE::detail

void HierarchySystem::OnUpdate(MyECS::Schedule& schedule) {
  GetWorld()->AddCommand([](MyECS::World* w) {
    auto hierarchy = w->entityMngr.GetSingleton<Hierarchy>();
    if (!hierarchy)
      return;
    if (ImGui::Begin("Hierarchy")) {

      MyECS::ArchetypeFilter filter;
      filter.none = {MyECS::CmptType::Of<Parent>};
      const_cast<MyECS::World*>(hierarchy->world)
          ->RunEntityJob(
              [=](MyECS::Entity e) {
                detail::HierarchyPrintEntity(hierarchy, e);
              },
              false, filter);
    }
    ImGui::End();
  });
}
