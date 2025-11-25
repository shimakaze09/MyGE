#include <MyGE/App/Editor/Components/Hierarchy.h>
#include <MyGE/App/Editor/Components/Inspector.h>
#include <MyGE/App/Editor/InspectorRegistry.h>
#include <MyGE/App/Editor/Systems/InspectorSystem.h>
#include <MyGE/Core/AssetMngr.h>
#include <_deps/imgui/imgui.h>

using namespace My::MyGE;

void InspectorSystem::OnUpdate(MyECS::Schedule& schedule) {
  schedule.GetWorld()->AddCommand(
      [world = schedule.GetWorld()]() {
        auto inspector = world->entityMngr.WriteSingleton<Inspector>();
        if (!inspector) return;

        auto hierarchy = world->entityMngr.WriteSingleton<Hierarchy>();
        switch (inspector->mode) {
          case Inspector::Mode::Asset: {
            auto path = AssetMngr::Instance().GUIDToAssetPath(inspector->asset);
            if (path.empty()) {
              inspector->asset = xg::Guid{};
              inspector->lock = false;
              return;
            }
            auto asset = AssetMngr::Instance().LoadMainAsset(path);
            const auto& type = AssetMngr::Instance().GetAssetType(path);

            if (ImGui::Begin("Inspector") && inspector->asset.isValid()) {
              ImGui::Checkbox("lock", &inspector->lock);
              ImGui::Separator();
              if (InspectorRegistry::Instance().IsRegistered(type))
                InspectorRegistry::Instance().Inspect(
                    world, type, asset.GetPtr());  // use self world?
            }
            ImGui::End();
            break;
          }
          case Inspector::Mode::Entity: {
            if (!hierarchy->world->entityMngr.Exist(inspector->entity)) {
              inspector->entity = MyECS::Entity::Invalid();
              inspector->lock = false;
            }

            if (ImGui::Begin("Inspector") && inspector->entity.Valid()) {
              if (ImGui::Button("Attach Component"))
                ImGui::OpenPopup("Attach Component Popup");

              if (ImGui::BeginPopup("Attach Component Popup")) {
                ImGui::PushID("Attach Component Popup");
                // Helper class to easy setup a text filter.
                // You may want to implement a more feature-full filtering
                // scheme in your own application.
                static ImGuiTextFilter filter;
                filter.Draw();
                int ID = 0;
                size_t N =
                    hierarchy->world->entityMngr.cmptTraits.GetNames().size();
                for (const auto& [type, name] :
                     hierarchy->world->entityMngr.cmptTraits.GetNames()) {
                  if (!type.Is<MyECS::Entity>() &&
                      !hierarchy->world->entityMngr.Have(inspector->entity,
                                                         type) &&
                      filter.PassFilter(name.data())) {
                    ImGui::PushID(ID);
                    ImGui::PushStyleColor(
                        ImGuiCol_Button,
                        (ImVec4)ImColor::HSV(ID / float(N), 0.6f, 0.6f));
                    ImGui::PushStyleColor(
                        ImGuiCol_ButtonHovered,
                        (ImVec4)ImColor::HSV(ID / float(N), 0.7f, 0.7f));
                    ImGui::PushStyleColor(
                        ImGuiCol_ButtonActive,
                        (ImVec4)ImColor::HSV(ID / float(N), 0.8f, 0.8f));
                    if (ImGui::Button(name.data()))
                      hierarchy->world->entityMngr.Attach(inspector->entity,
                                                          TempTypeIDs{type});
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                  }
                  ID++;
                }
                ImGui::PopID();
                ImGui::EndPopup();
              }
              ImGui::SameLine();
              if (ImGui::Button("Detach Component"))
                ImGui::OpenPopup("Detach Component Popup");
              if (ImGui::BeginPopup("Detach Component Popup")) {
                ImGui::PushID("Detach Component Popup");
                // Helper class to easy setup a text filter.
                // You may want to implement a more feature-full filtering
                // scheme in your own application.
                static ImGuiTextFilter filter;
                filter.Draw();
                int ID = 0;
                auto cmpts = hierarchy->world->entityMngr.Components(
                    inspector->entity, MyECS::AccessMode::WRITE);
                size_t N = cmpts.size();
                for (const auto& cmpt : cmpts) {
                  auto name = hierarchy->world->entityMngr.cmptTraits.Nameof(
                      cmpt.AccessType());
                  if (!name.empty()) {
                    if (filter.PassFilter(name.data())) {
                      ImGui::PushID(ID);
                      ImGui::PushStyleColor(
                          ImGuiCol_Button,
                          (ImVec4)ImColor::HSV(ID / float(N), 0.6f, 0.6f));
                      ImGui::PushStyleColor(
                          ImGuiCol_ButtonHovered,
                          (ImVec4)ImColor::HSV(ID / float(N), 0.7f, 0.7f));
                      ImGui::PushStyleColor(
                          ImGuiCol_ButtonActive,
                          (ImVec4)ImColor::HSV(ID / float(N), 0.8f, 0.8f));
                      if (ImGui::Button(name.data())) {
                        const auto cmptType = cmpt.AccessType();
                        hierarchy->world->entityMngr.Detach(
                            inspector->entity, TempTypeIDs{cmptType});
                      }
                      ImGui::PopStyleColor(3);
                      ImGui::PopID();
                    }
                  } else {
                    auto name = std::to_string(cmpt.AccessType().GetValue());
                    if (filter.PassFilter(name.c_str())) {
                      ImGui::PushID(ID);
                      ImGui::PushStyleColor(
                          ImGuiCol_Button,
                          (ImVec4)ImColor::HSV(ID / float(N), 0.6f, 0.6f));
                      ImGui::PushStyleColor(
                          ImGuiCol_ButtonHovered,
                          (ImVec4)ImColor::HSV(ID / float(N), 0.7f, 0.7f));
                      ImGui::PushStyleColor(
                          ImGuiCol_ButtonActive,
                          (ImVec4)ImColor::HSV(ID / float(N), 0.8f, 0.8f));
                      if (ImGui::Button(name.c_str())) {
                        const auto cmptType = cmpt.AccessType();
                        hierarchy->world->entityMngr.Detach(
                            inspector->entity, TempTypeIDs{cmptType});
                      }
                      ImGui::PopStyleColor(3);
                      ImGui::PopID();
                    }
                  }
                  ID++;
                }
                ImGui::PopID();
                ImGui::EndPopup();
              }
              ImGui::SameLine();
              ImGui::Checkbox("lock", &inspector->lock);
              ImGui::Separator();

              auto cmpts = hierarchy->world->entityMngr.Components(
                  inspector->entity, MyECS::AccessMode::WRITE);
              for (size_t i = 0; i < cmpts.size(); i++) {
                auto type = cmpts[i].AccessType();
                if (InspectorRegistry::Instance().IsRegistered(type)) continue;
                auto name =
                    hierarchy->world->entityMngr.cmptTraits.Nameof(type);
                if (name.empty())
                  ImGui::Text(std::to_string(type.GetValue()).c_str());
                else
                  ImGui::Text(name.data());
              }

              for (const auto& cmpt : cmpts) {
                if (InspectorRegistry::Instance().IsRegistered(
                        cmpt.AccessType()))
                  InspectorRegistry::Instance().InspectComponent(
                      hierarchy->world, MyECS::CmptPtr{cmpt});
              }
            }
            ImGui::End();
            break;
          }
          default:
            assert(false);
            break;
        }
      },
      0);
}

