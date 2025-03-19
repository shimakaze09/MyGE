//
// Created by Admin on 19/03/2025.
//

#include "InspectorSystem.h"

#include "../CmptInsepctor.h"

#include "../Components/Hierarchy.h"
#include "../Components/Inspector.h"

#include <MyGE/Transform/Components/Components.h>

#include <MyGE/_deps/imgui/imgui.h>

using namespace My::MyGE;

void InspectorSystem::OnUpdate(MyECS::Schedule& schedule) {
  schedule.RegisterJob(
      [](MyECS::World* w, MyECS::Latest<MyECS::Singleton<Hierarchy>> hierarchy,
         MyECS::Singleton<Inspector> inspector) {
        if (!inspector->lock)
          inspector->target = hierarchy->select;

        w->AddCommand([inspector, world = hierarchy->world](MyECS::World*) {
          if (ImGui::Begin("Inspector") && inspector->target.Valid()) {
            auto cmpts = world->entityMngr.Components(inspector->target);
            for (size_t i = 0; i < cmpts.size(); i++) {
              auto type = cmpts[i].Type();
              if (CmptInspector::Instance().IsCmptRegistered(type))
                continue;
              auto name = world->cmptTraits.Nameof(type);
              if (name.empty())
                ImGui::Text(std::to_string(type.HashCode()).c_str());
              else
                ImGui::Text(name.data());
            }

            for (const auto& cmpt : cmpts) {
              if (CmptInspector::Instance().IsCmptRegistered(cmpt.Type()))
                CmptInspector::Instance().Inspect(cmpt);
            }
          }
          ImGui::End();
        });
      },
      "InspectorSystem");
}
