#include <MyGE/App/Editor/Systems/HierarchySystem.h>

#include <MyGE/App/Editor/PlayloadType.h>

#include <MyGE/App/Editor/Components/Hierarchy.h>
#include <MyGE/App/Editor/Components/Inspector.h>

#include <MyGE/Core/Components/Children.h>
#include <MyGE/Core/Components/Name.h>
#include <MyGE/Core/Components/Parent.h>

#include <_deps/imgui/imgui.h>

using namespace My::MyGE;

namespace My::MyGE::detail {
bool HierarchyMovable(const MyECS::World* w, MyECS::Entity dst,
                      MyECS::Entity src) {
  if (dst == src)
    return false;
  else if (auto p = w->entityMngr.Get<Parent>(dst))
    return HierarchyMovable(w, p->value, src);
  else
    return true;
}

void HierarchyPrintEntity(Hierarchy* hierarchy, MyECS::Entity e,
                          Inspector* inspector) {
  static constexpr ImGuiTreeNodeFlags nodeBaseFlags =
      ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
      ImGuiTreeNodeFlags_SpanAvailWidth;

  auto children = hierarchy->world->entityMngr.Get<Children>(e);
  auto name = hierarchy->world->entityMngr.Get<Name>(e);
  bool isLeaf = !children || children->value.empty();

  ImGuiTreeNodeFlags nodeFlags = nodeBaseFlags;
  if (hierarchy->select == e)
    nodeFlags |= ImGuiTreeNodeFlags_Selected;
  if (isLeaf)
    nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

  bool nodeOpen =
      name ? ImGui::TreeNodeEx((void*)(intptr_t)e.Idx(), nodeFlags, "%s (%d)",
                               name->value.c_str(), e.Idx())
           : ImGui::TreeNodeEx((void*)(intptr_t)e.Idx(), nodeFlags,
                               "Entity (%d)", e.Idx());

  if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
    ImGui::SetDragDropPayload(PlayloadType::ENTITY, &e, sizeof(MyECS::Entity));
    if (name)
      ImGui::Text("%s (%d)", name->value.c_str(), e.Idx());
    else
      ImGui::Text("Entity (%d)", e.Idx());

    ImGui::EndDragDropSource();
  }

  if (ImGui::BeginDragDropTarget()) {
    if (const ImGuiPayload* payload =
            ImGui::AcceptDragDropPayload(PlayloadType::ENTITY)) {
      IM_ASSERT(payload->DataSize == sizeof(MyECS::Entity));
      auto payload_e = *(const MyECS::Entity*)payload->Data;
      if (HierarchyMovable(hierarchy->world, e, payload_e)) {
        auto [payload_e_p] =
            hierarchy->world->entityMngr.Attach<Parent>(payload_e);
        if (payload_e_p->value.Valid()) {
          auto parentChildren =
              hierarchy->world->entityMngr.Get<Children>(payload_e_p->value);
          parentChildren->value.erase(payload_e);
        }
        payload_e_p->value = e;
        auto [children] = hierarchy->world->entityMngr.Attach<Children>(e);
        children->value.insert(payload_e);
      }
    }
    ImGui::EndDragDropTarget();
  }

  if (ImGui::IsItemHovered() && ImGui::IsItemDeactivated()) {
    hierarchy->select = e;
    if (inspector && !inspector->lock) {
      inspector->mode = Inspector::Mode::Entity;
      inspector->entity = e;
    }
  }
  if (ImGui::IsItemHovered())
    hierarchy->hover = e;

  if (nodeOpen && !isLeaf) {
    for (const auto& child : children->value)
      HierarchyPrintEntity(hierarchy, child, inspector);
    ImGui::TreePop();
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
    children->value.erase(e);
  }
  detail::HierarchyDeleteEntityRecursively(w, e);
}
}  // namespace My::MyGE::detail

void HierarchySystem::OnUpdate(MyECS::Schedule& schedule) {
  schedule.RegisterCommand([](MyECS::World* w) {
    auto hierarchy = w->entityMngr.GetSingleton<Hierarchy>();
    if (!hierarchy)
      return;

    if (ImGui::Begin("Hierarchy")) {
      if (ImGui::IsWindowHovered() &&
          ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        ImGui::OpenPopup("Hierarchy_popup");

      if (ImGui::BeginPopup("Hierarchy_popup")) {
        if (hierarchy->hover.Valid()) {
          if (ImGui::MenuItem("Create Empty")) {
            auto [e, p] = hierarchy->world->entityMngr.Create<Parent>();
            p->value = hierarchy->hover;
            auto [children] =
                hierarchy->world->entityMngr.Attach<Children>(hierarchy->hover);
            children->value.insert(e);
          }

          if (ImGui::MenuItem("Delete")) {
            detail::HierarchyDeleteEntity(hierarchy->world, hierarchy->hover);

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

      if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload =
                ImGui::AcceptDragDropPayload(PlayloadType::ENTITY)) {
          IM_ASSERT(payload->DataSize == sizeof(MyECS::Entity));
          auto payload_e = *(const MyECS::Entity*)payload->Data;

          if (auto payload_e_p =
                  hierarchy->world->entityMngr.Get<Parent>(payload_e)) {
            auto parentChildren =
                hierarchy->world->entityMngr.Get<Children>(payload_e_p->value);
            parentChildren->value.erase(payload_e);
            hierarchy->world->entityMngr.Detach(
                payload_e, &MyECS::CmptType::Of<Parent>, 1);
          }
        }
        ImGui::EndDragDropTarget();
      }

      auto inspector = w->entityMngr.GetSingleton<Inspector>();
      MyECS::ArchetypeFilter filter;
      filter.none = {MyECS::CmptType::Of<Parent>};
      hierarchy->world->RunEntityJob(
          [=](MyECS::Entity e) {
            detail::HierarchyPrintEntity(hierarchy, e, inspector);
          },
          false, filter);
    }
    ImGui::End();
  });
}
