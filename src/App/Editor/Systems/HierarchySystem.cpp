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
    if (ImGui::IsItemHovered())
      hierarchy->hover = e;
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
    if (ImGui::IsItemHovered())
      hierarchy->hover = e;
  }
}

// delete e and his children
void HierarchyDeleteEntityRecursively(MyECS::World* w, MyECS::Entity e) {
  if (auto children = w->entityMngr.Get<Children>(e)) {
    for (const auto& child : children->value)
      HierarchyDeleteEntityRecursively(w, child);
  }
  w->entityMngr.Destroy(e);
}

// delete e in it's parent
// then call HierarchyDeleteEntityRecursively
void HierarchyDeleteEntity(MyECS::World* w, MyECS::Entity e) {
  if (auto p = w->entityMngr.Get<Parent>(e)) {
    auto e_p = p->value;
    auto children = w->entityMngr.Get<Children>(e_p);
    children->value.erase(e_p);
  }
  detail::HierarchyDeleteEntityRecursively(w, e);
}
}  // namespace My::MyGE::detail

void HierarchySystem::OnUpdate(MyECS::Schedule& schedule) {
  schedule.RegisterJob(
      [](MyECS::World* w,
         MyECS::Latest<MyECS::Singleton<Hierarchy>> hierarchy) {
        w->AddCommand([hierarchy = const_cast<Hierarchy*>(hierarchy.Get())](
                          MyECS::World* w) {
          if (ImGui::Begin("Hierarchy")) {
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) &&
                ImGui::IsWindowHovered())
              ImGui::OpenPopup("Hierarchy_popup");

            if (ImGui::BeginPopup("Hierarchy_popup")) {
              if (hierarchy->hover.Valid()) {
                if (ImGui::MenuItem("Create Empty")) {
                  auto [e, p] = hierarchy->world->entityMngr.Create<Parent>();
                  p->value = hierarchy->hover;
                  auto [children] =
                      hierarchy->world->entityMngr.Attach<Children>(
                          hierarchy->hover);
                  children->value.insert(e);
                }

                if (ImGui::MenuItem("Delete")) {
                  detail::HierarchyDeleteEntity(hierarchy->world,
                                                hierarchy->hover);

                  hierarchy->hover = MyECS::Entity::Invalid();
                  if (!hierarchy->world->entityMngr.Exist(hierarchy->select))
                    hierarchy->select = MyECS::Entity::Invalid();
                }
              } else {
                if (ImGui::MenuItem("Create Empty Entity"))
                  hierarchy->world->entityMngr.Create();
              }

              ImGui::EndPopup();
            } else
              hierarchy->hover = MyECS::Entity::Invalid();

            MyECS::ArchetypeFilter filter;
            filter.none = {MyECS::CmptType::Of<Parent>};
            hierarchy->world->RunEntityJob(
                [=](MyECS::Entity e) {
                  detail::HierarchyPrintEntity(hierarchy, e);
                },
                false, filter);
          }
          ImGui::End();
        });
      },
      "HierarchySystem");
}
