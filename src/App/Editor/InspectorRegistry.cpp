#include <MyGE/App/Editor/InspectorRegistry.h>
#include <MyGE/Core/AssetMngr.h>
#include <MyGE/Core/Components/Name.h>
#include <_deps/imgui/imgui.h>
#include <_deps/imgui/misc/cpp/imgui_stdlib.h>

#include <MyDRefl/MyDRefl.hpp>
#include <MyTemplate/Type.hpp>
#include <functional>
#include <unordered_map>

using namespace My::MyDRefl;
using namespace My::MyGE;

struct InspectorRegistry::Impl {
  Visitor<void(void*, InspectContext)> inspector;
};

InspectorRegistry::InspectorRegistry() : pImpl{new Impl} {}
InspectorRegistry::~InspectorRegistry() { delete pImpl; }

void InspectorRegistry::Register(
    TypeID type, std::function<void(void*, InspectContext)> cmptInspectFunc) {
  pImpl->inspector.Register(type.GetValue(), std::move(cmptInspectFunc));
}

bool InspectorRegistry::IsRegistered(TypeID type) const {
  return pImpl->inspector.IsRegistered(type.GetValue()) ||
         MyDRefl::Mngr.typeinfos.contains(MyDRefl::Mngr.tregistry.Typeof(type));
}

bool InspectorRegistry::IsRegistered(Type type) const {
  return pImpl->inspector.IsRegistered(type.GetID().GetValue()) ||
         MyDRefl::Mngr.typeinfos.contains(type);
}

void InspectorRegistry::InspectComponent(const MyECS::World* world,
                                         MyECS::CmptPtr cmpt) {
  if (pImpl->inspector.IsRegistered(cmpt.Type().GetValue())) {
    pImpl->inspector.Visit(cmpt.Type().GetValue(), cmpt.Ptr(),
                           InspectContext{world, pImpl->inspector});
    return;
  }

  ImGui::PushID((const void*)cmpt.Type().GetValue());

  auto type = MyDRefl::Mngr.tregistry.Typeof(cmpt.Type());
  if (ImGui::CollapsingHeader(type.GetName().data())) {
    InspectorRegistry::InspectContext ctx{world, pImpl->inspector};
    for (const auto& [n, var] : VarRange{ObjectView{type, cmpt.Ptr()}})
      InspectRecursively(n, var.GetType().GetID(), var.GetPtr(), ctx);
  }

  ImGui::PopID();
}

void InspectorRegistry::Inspect(const MyECS::World* w, TypeID typeID,
                                void* obj) {
  if (pImpl->inspector.IsRegistered(typeID.GetValue())) {
    pImpl->inspector.Visit(typeID.GetValue(), obj,
                           InspectContext{nullptr, pImpl->inspector});
    return;
  }

  if (AssetMngr::Instance().Contains(obj)) {
    auto guid = AssetMngr::Instance().GetAssetGUID(obj);
    const auto& path = AssetMngr::Instance().GetAssetPath(obj);
    auto importer = AssetMngr::Instance().GetImporter(path);
    auto objv = importer->This();
    if (ImGui::CollapsingHeader(objv.GetType().GetName().data())) {
      InspectorRegistry::InspectContext ctx{w, pImpl->inspector};
      for (const auto& [n, var] : objv.GetVars())
        InspectRecursively(n, var.GetType().GetID(), var.GetPtr(), ctx);
      if (ImGui::Button("apply"))
        AssetMngr::Instance().SetImporterOverride(path, importer);
    }
    auto assets = AssetMngr::Instance().LoadAllAssets(path);
    for (const auto& asset : assets) {
      ImGui::PushID(asset.GetPtr());
      if (ImGui::CollapsingHeader(asset.GetType().GetName().data())) {
        InspectorRegistry::InspectContext ctx{w, pImpl->inspector};
        for (const auto& [n, var] : asset.GetVars())
          InspectRecursively(n, var.GetType().GetID(), var.GetPtr(), ctx);
        if (ImGui::Button("apply"))
          AssetMngr::Instance().ReserializeAsset(path);
      }
      ImGui::PopID();
    }
  } else {
    auto type = MyDRefl::Mngr.tregistry.Typeof(typeID);
    InspectorRegistry::InspectContext ctx{w, pImpl->inspector};
    for (const auto& [n, var] : VarRange{ObjectView{type, obj}})
      InspectRecursively(n, var.GetType().GetID(), var.GetPtr(), ctx);
  }
}

void InspectorRegistry::InspectRecursively(std::string_view name, TypeID typeID,
                                           void* obj, InspectContext ctx) {
  if (ctx.inspector.IsRegistered(typeID.GetValue())) {
    ctx.inspector.Visit(typeID.GetValue(), obj, ctx);
    return;
  }

  auto type = MyDRefl::Mngr.tregistry.Typeof(typeID);
  if (!type.Valid()) return;

  MyDRefl::ObjectView objv{type, obj};
  InspectRecursively(name, objv, ctx);
}

void InspectorRegistry::InspectRecursively(std::string_view name,
                                           MyDRefl::ObjectView objv,
                                           InspectContext ctx) {
  if (ctx.inspector.IsRegistered(objv.GetType().GetID().GetValue())) {
    ctx.inspector.Visit(objv.GetType().GetID().GetValue(), objv.GetPtr(), ctx);
    return;
  }

  auto type = objv.GetType();
  if (!type.Valid()) return;

  if (type.IsConst()) {
    objv = objv.RemoveConst();
    type = objv.GetType();

    if (type.IsArithmetic()) {
      switch (type.GetID().GetValue()) {
        case TypeID_of<bool>.GetValue():
          ImGui::Text("%s", objv.As<bool>() ? "true" : "false");
          break;
        case TypeID_of<std::int8_t>.GetValue():
          ImGui::Text("%d", static_cast<int>(objv.As<std::int8_t>()));
          break;
        case TypeID_of<std::int16_t>.GetValue():
          ImGui::Text("%d", static_cast<int>(objv.As<std::int16_t>()));
          break;
        case TypeID_of<std::int32_t>.GetValue():
          ImGui::Text("%d", static_cast<int>(objv.As<std::int32_t>()));
          break;
        case TypeID_of<std::int64_t>.GetValue():
          ImGui::Text("%I64d", objv.As<std::int64_t>());
          break;
        case TypeID_of<std::uint8_t>.GetValue():
          ImGui::Text("%ud",
                      static_cast<unsigned int>(objv.As<std::uint8_t>()));
          break;
        case TypeID_of<std::uint16_t>.GetValue():
          ImGui::Text("%ud",
                      static_cast<unsigned int>(objv.As<std::uint16_t>()));
          break;
        case TypeID_of<std::uint32_t>.GetValue():
          ImGui::Text("%ud",
                      static_cast<unsigned int>(objv.As<std::uint32_t>()));
          break;
        case TypeID_of<std::uint64_t>.GetValue():
          ImGui::Text("%zu", objv.As<std::uint64_t>());
          break;
        case TypeID_of<float>.GetValue():
          ImGui::Text("%f", objv.As<float>());
          break;
        case TypeID_of<double>.GetValue():
          ImGui::Text("%lf", objv.As<double>());
          break;
        default:
          assert(false);
          break;
      }
      ImGui::SameLine();
      ImGui::Text(name.data());
    } else if (type.IsEnum()) {
      std::string_view cur;
      for (const auto& [n, var] : VarRange{objv.GetType()}) {
        if (var == objv) {
          cur = n;
          break;
        }
      }
      assert(!cur.empty());
      ImGui::Text(cur.data());
      ImGui::SameLine();
      ImGui::Text(name.data());
    } else if (type.Is<std::string>()) {
      ImGui::Text(objv.As<std::string>().data());
      ImGui::SameLine();
      ImGui::Text(name.data());
    } else if (type.Is<std::pmr::string>()) {
      ImGui::Text(objv.As<std::pmr::string>().data());
      ImGui::SameLine();
      ImGui::Text(name.data());
    } else if (type.Is<std::string_view>()) {
      ImGui::Text(objv.As<std::string_view>().data());
      ImGui::SameLine();
      ImGui::Text(name.data());
    } else if (type.Is<SharedObject>()) {
      ImGui::Text("(*)");
      ImGui::SameLine();

      {  // button
        auto sobj = objv.As<SharedObject>();
        // button
        if (sobj.GetPtr()) {
          const auto& path = AssetMngr::Instance().GetAssetPath(sobj.GetPtr());
          if (!path.empty()) {
            auto name = path.stem().string();
            ImGui::Button(name.c_str());
          } else
            ImGui::Button("UNKNOW");
        } else
          ImGui::Button("nullptr");
      }

      ImGui::SameLine();
      ImGui::Text(name.data());
    } else if (type.GetName().starts_with("My::MyGE::SharedVar<")) {
      ImGui::Text("(*)");
      ImGui::SameLine();

      auto sobj = objv.Invoke("cast_to_shared_obj");
      {  // button
        // button
        if (sobj.GetPtr()) {
          const auto& path = AssetMngr::Instance().GetAssetPath(sobj.GetPtr());
          if (!path.empty()) {
            auto name = path.stem().string();
            ImGui::Button(name.c_str());
          } else
            ImGui::Button("UNKNOW");
        } else
          ImGui::Button("nullptr");
      }
      ImGui::SameLine();
      ImGui::Text(name.data());
    } else if (auto attr = Mngr.GetTypeAttr(type, Type_of<ContainerType>);
               attr.GetType().Valid()) {
      ContainerType ct = attr.As<ContainerType>();
      switch (ct) {
        case My::MyDRefl::ContainerType::Stack:
        case My::MyDRefl::ContainerType::Queue:
        case My::MyDRefl::ContainerType::PriorityQueue:
        case My::MyDRefl::ContainerType::None:
          break;
        case My::MyDRefl::ContainerType::Array:
        case My::MyDRefl::ContainerType::RawArray: {  // valf1, valf2, valf3,
                                                        // valf4, ...
                                                        // TODO
        }
          [[fallthrough]];
        case My::MyDRefl::ContainerType::Span:
        case My::MyDRefl::ContainerType::Vector:
        case My::MyDRefl::ContainerType::Deque:
        case My::MyDRefl::ContainerType::ForwardList:
        case My::MyDRefl::ContainerType::List:
        case My::MyDRefl::ContainerType::MultiSet:
        case My::MyDRefl::ContainerType::Map:
        case My::MyDRefl::ContainerType::MultiMap:
        case My::MyDRefl::ContainerType::Set:
        case My::MyDRefl::ContainerType::UnorderedMap:
        case My::MyDRefl::ContainerType::UnorderedMultiSet:
        case My::MyDRefl::ContainerType::UnorderedMultiMap:
        case My::MyDRefl::ContainerType::UnorderedSet: {
          if (ImGui::TreeNodeEx(name.data(), ImGuiTreeNodeFlags_DefaultOpen)) {
            std::size_t i = 0;
            auto e = objv.end();
            for (auto iter = objv.begin(); iter != e; ++iter) {
              auto n = std::to_string(i++);
              auto v = (*iter).RemoveReference().AddConst();
              InspectRecursively(n, v, ctx);
            }
            ImGui::TreePop();
          }
        } break;
        case My::MyDRefl::ContainerType::Pair:
        case My::MyDRefl::ContainerType::Tuple: {
          if (ImGui::TreeNodeEx(name.data(), ImGuiTreeNodeFlags_DefaultOpen)) {
            std::size_t size = objv.tuple_size();
            std::size_t i = 0;
            for (std::size_t i = 0; i < size; i++) {
              auto n = std::to_string(i);
              auto v = objv.get(i).RemoveReference().AddConst();
              InspectRecursively(n, v, ctx);
            }
            ImGui::TreePop();
          }
        } break;
        case My::MyDRefl::ContainerType::Variant: {
          auto v = objv.variant_visit_get().RemoveReference().AddConst();
          InspectRecursively(name, v, ctx);
        } break;
        case My::MyDRefl::ContainerType::Optional: {
          if (objv.has_value()) {
            auto v = objv.value().RemoveReference().AddConst();
            InspectRecursively(name, v, ctx);
          } else
            ImGui::Text("null");
        } break;
        default:
          assert(false);
          break;
      }
    } else {
      if (ImGui::TreeNodeEx(name.data(), ImGuiTreeNodeFlags_DefaultOpen)) {
        for (const auto& [n, var] :
             VarRange{objv.AddConst(), FieldFlag::Owned}) {
          assert(var.GetType().IsConst());
          InspectRecursively(n, var, ctx);
        }
        ImGui::TreePop();
      }
    }
  } else {
    if (type.IsArithmetic()) {
      switch (type.GetID().GetValue()) {
        case TypeID_of<bool>.GetValue():
          ImGui::Checkbox(name.data(), &objv.As<bool>());
          break;
        case TypeID_of<std::int8_t>.GetValue():
          ImGui::DragScalarN(name.data(), ImGuiDataType_S8, objv.GetPtr(), 1,
                             1.f);
          break;
        case TypeID_of<std::int16_t>.GetValue():
          ImGui::DragScalarN(name.data(), ImGuiDataType_S16, objv.GetPtr(), 1,
                             1.f);
          break;
        case TypeID_of<std::int32_t>.GetValue():
          ImGui::DragScalarN(name.data(), ImGuiDataType_S32, objv.GetPtr(), 1,
                             1.f);
          break;
        case TypeID_of<std::int64_t>.GetValue():
          ImGui::DragScalarN(name.data(), ImGuiDataType_S64, objv.GetPtr(), 1,
                             1.f);
          break;
        case TypeID_of<std::uint8_t>.GetValue():
          ImGui::DragScalarN(name.data(), ImGuiDataType_U8, objv.GetPtr(), 1,
                             1.f);
          break;
        case TypeID_of<std::uint16_t>.GetValue():
          ImGui::DragScalarN(name.data(), ImGuiDataType_U16, objv.GetPtr(), 1,
                             1.f);
          break;
        case TypeID_of<std::uint32_t>.GetValue():
          ImGui::DragScalarN(name.data(), ImGuiDataType_U32, objv.GetPtr(), 1,
                             1.f);
          break;
        case TypeID_of<std::uint64_t>.GetValue():
          ImGui::DragScalarN(name.data(), ImGuiDataType_U64, objv.GetPtr(), 1,
                             1.f);
          break;
        case TypeID_of<float>.GetValue():
          ImGui::DragScalarN(name.data(), ImGuiDataType_Float, objv.GetPtr(), 1,
                             0.1f);
          break;
        case TypeID_of<double>.GetValue():
          ImGui::DragScalarN(name.data(), ImGuiDataType_Double, objv.GetPtr(),
                             1, 0.1f);
          break;
        default:
          assert(false);
          break;
      }
    } else if (type.IsEnum()) {
      std::string_view cur;
      for (const auto& [n, var] : objv.GetVars(FieldFlag::Unowned)) {
        if (var == objv) {
          cur = n;
          break;
        }
      }
      assert(!cur.empty());
      if (ImGui::BeginCombo(name.data(), cur.data())) {
        for (const auto& [n, var] : VarRange{objv.GetType()}) {
          bool isSelected = var == objv;
          if (ImGui::Selectable(n.GetView().data(), isSelected))
            objv.Invoke<void>(NameIDRegistry::Meta::operator_assignment,
                              TempArgsView{var}, MethodFlag::Variable);

          if (isSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
    } else if (type.Is<std::string>())
      ImGui::InputText(name.data(), &objv.As<std::string>());
    else if (type.Is<std::pmr::string>()) {
      std::string str{objv.As<std::pmr::string>()};
      if (ImGui::InputText(name.data(), &str))
        objv.As<std::pmr::string>() = str;
    } else if (type.Is<std::string_view>())
      ImGui::Text(objv.As<std::string_view>().data());
    else if (type.Is<SharedObject>()) {
      ImGui::Text("(*)");
      ImGui::SameLine();

      {  // button
        auto sobj = objv.As<SharedObject>();
        // button
        if (sobj.GetPtr()) {
          const auto& path = AssetMngr::Instance().GetAssetPath(sobj.GetPtr());
          if (!path.empty()) {
            auto name = path.stem().string();
            ImGui::Button(name.c_str());
          } else
            ImGui::Button("UNKNOW");
        } else
          ImGui::Button("nullptr");
      }

      if (ImGui::BeginDragDropTarget()) {  // to button
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(
                InspectorRegistry::Playload::Asset)) {
          IM_ASSERT(payload->DataSize ==
                    sizeof(InspectorRegistry::Playload::AssetHandle));
          auto assethandle =
              *(InspectorRegistry::Playload::AssetHandle*)payload->Data;
          SharedObject asset;
          if (assethandle.name.empty()) {
            // main
            asset = AssetMngr::Instance().GUIDToAsset(assethandle.guid);
          } else
            asset = AssetMngr::Instance().GUIDToAsset(assethandle.guid,
                                                      assethandle.name);
          objv.Invoke<void>(
              NameIDRegistry::Meta::operator_assignment,
              TempArgsView{ObjectView{Type_of<SharedObject>, &asset}},
              MethodFlag::Variable);
        }
        ImGui::EndDragDropTarget();
      }
      ImGui::SameLine();
      ImGui::Text(name.data());
    } else if (type.GetName().starts_with("My::MyGE::SharedVar<")) {
      ImGui::Text("(*)");
      ImGui::SameLine();

      auto sobj = objv.Invoke("cast_to_shared_obj");
      {  // button
        // button
        if (sobj.GetPtr()) {
          const auto& path = AssetMngr::Instance().GetAssetPath(sobj.GetPtr());
          if (!path.empty()) {
            auto name = path.stem().string();
            ImGui::Button(name.c_str());
          } else
            ImGui::Button("UNKNOW");
        } else
          ImGui::Button("nullptr");
      }

      if (ImGui::BeginDragDropTarget()) {  // to button
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(
                InspectorRegistry::Playload::Asset)) {
          IM_ASSERT(payload->DataSize ==
                    sizeof(InspectorRegistry::Playload::AssetHandle));
          auto assethandle =
              *(InspectorRegistry::Playload::AssetHandle*)payload->Data;
          SharedObject asset;
          if (assethandle.name.empty()) {
            // main
            asset = AssetMngr::Instance().GUIDToAsset(assethandle.guid);
          } else
            asset = AssetMngr::Instance().GUIDToAsset(assethandle.guid,
                                                      assethandle.name);
          objv.Invoke<void>(
              NameIDRegistry::Meta::operator_assignment,
              TempArgsView{ObjectView{Type_of<SharedObject>, &asset}},
              MethodFlag::Variable);
        }
        ImGui::EndDragDropTarget();
      }
      ImGui::SameLine();
      ImGui::Text(name.data());
    } else if (auto attr = Mngr.GetTypeAttr(type, Type_of<ContainerType>);
               attr.GetType().Valid()) {
      ContainerType ct = attr.As<ContainerType>();
      switch (ct) {
        case My::MyDRefl::ContainerType::Stack:
        case My::MyDRefl::ContainerType::Queue:
        case My::MyDRefl::ContainerType::PriorityQueue:
        case My::MyDRefl::ContainerType::None:
          break;
        case My::MyDRefl::ContainerType::Array:
        case My::MyDRefl::ContainerType::RawArray: {  // valf1, valf2, valf3,
                                                        // valf4, ...
          auto N = static_cast<int>(objv.size());
          if (N >= 1 && N <= 4) {
            auto type = objv[0].RemoveReference().GetType();
            if (type.IsArithmetic()) {
              switch (type.GetID().GetValue()) {
                case TypeID_of<std::int8_t>.GetValue():
                  ImGui::DragScalarN(name.data(), ImGuiDataType_S8,
                                     objv.GetPtr(), N, 1.f);
                  break;
                case TypeID_of<std::int16_t>.GetValue():
                  ImGui::DragScalarN(name.data(), ImGuiDataType_S16,
                                     objv.GetPtr(), N, 1.f);
                  break;
                case TypeID_of<std::int32_t>.GetValue():
                  ImGui::DragScalarN(name.data(), ImGuiDataType_S32,
                                     objv.GetPtr(), N, 1.f);
                  break;
                case TypeID_of<std::int64_t>.GetValue():
                  ImGui::DragScalarN(name.data(), ImGuiDataType_S64,
                                     objv.GetPtr(), N, 1.f);
                  break;
                case TypeID_of<std::uint8_t>.GetValue():
                  ImGui::DragScalarN(name.data(), ImGuiDataType_U8,
                                     objv.GetPtr(), N, 1.f);
                  break;
                case TypeID_of<std::uint16_t>.GetValue():
                  ImGui::DragScalarN(name.data(), ImGuiDataType_U16,
                                     objv.GetPtr(), N, 1.f);
                  break;
                case TypeID_of<std::uint32_t>.GetValue():
                  ImGui::DragScalarN(name.data(), ImGuiDataType_U32,
                                     objv.GetPtr(), N, 1.f);
                  break;
                case TypeID_of<std::uint64_t>.GetValue():
                  ImGui::DragScalarN(name.data(), ImGuiDataType_U64,
                                     objv.GetPtr(), N, 1.f);
                  break;
                case TypeID_of<float>.GetValue():
                  ImGui::DragScalarN(name.data(), ImGuiDataType_Float,
                                     objv.GetPtr(), N, 0.1f);
                  break;
                case TypeID_of<double>.GetValue():
                  ImGui::DragScalarN(name.data(), ImGuiDataType_Double,
                                     objv.GetPtr(), N, 0.1f);
                  break;
                default:
                  assert(false);
                  break;
              }
              break;
            }
          }
        }
          [[fallthrough]];
        case My::MyDRefl::ContainerType::Vector:
        case My::MyDRefl::ContainerType::Deque:
        case My::MyDRefl::ContainerType::ForwardList:  // TODO: append
        case My::MyDRefl::ContainerType::List:
        case My::MyDRefl::ContainerType::Span:
        case My::MyDRefl::ContainerType::MultiSet:
        case My::MyDRefl::ContainerType::Map:
        case My::MyDRefl::ContainerType::MultiMap:
        case My::MyDRefl::ContainerType::Set:
        case My::MyDRefl::ContainerType::UnorderedMap:
        case My::MyDRefl::ContainerType::UnorderedMultiSet:
        case My::MyDRefl::ContainerType::UnorderedMultiMap:
        case My::MyDRefl::ContainerType::UnorderedSet: {
          if (ImGui::TreeNodeEx(name.data(), ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ct == My::MyDRefl::ContainerType::Vector ||
                ct == My::MyDRefl::ContainerType::Deque ||
                ct == My::MyDRefl::ContainerType::ForwardList ||
                ct == My::MyDRefl::ContainerType::List) {
              int s = static_cast<int>(objv.size());
              int origs = s;
              ImGui::InputInt("size", &s, 1);
              if (s != origs && s >= 0) objv.resize(static_cast<size_t>(s));
            }

            std::size_t i = 0;
            auto e = objv.end();
            for (auto iter = objv.begin(); iter != e; ++iter) {
              auto n = std::to_string(i++);
              auto v = (*iter).RemoveReference();
              InspectRecursively(n, v, ctx);
            }
            ImGui::TreePop();
          }
        } break;
        case My::MyDRefl::ContainerType::Pair:
        case My::MyDRefl::ContainerType::Tuple: {
          if (ImGui::TreeNodeEx(name.data(), ImGuiTreeNodeFlags_DefaultOpen)) {
            std::size_t size = objv.tuple_size();
            std::size_t i = 0;
            for (std::size_t i = 0; i < size; i++) {
              auto n = std::to_string(i);
              auto v = objv.get(i).RemoveReference();
              InspectRecursively(n, v, ctx);
            }
            ImGui::TreePop();
          }
        } break;
        case My::MyDRefl::ContainerType::Variant: {
          auto v = objv.variant_visit_get().RemoveReference();
          InspectRecursively(name, v, ctx);
        } break;
        case My::MyDRefl::ContainerType::Optional: {
          if (objv.has_value()) {
            auto v = objv.value().RemoveReference();
            InspectRecursively(name, v, ctx);
          } else
            ImGui::Text("null");
        } break;
        default:
          assert(false);
          break;
      }
    } else {
      if (ImGui::TreeNodeEx(name.data(), ImGuiTreeNodeFlags_DefaultOpen)) {
        for (const auto& [n, var] : VarRange{objv, FieldFlag::Owned})
          InspectRecursively(n, var, ctx);
        ImGui::TreePop();
      }
    }
  }
}

