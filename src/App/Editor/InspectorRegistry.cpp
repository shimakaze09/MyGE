#include <MyGE/App/Editor/InspectorRegistry.h>

#include <MyTemplate/Type.hpp>
#include <functional>
#include <unordered_map>

using namespace Smkz::MyGE;

struct InspectorRegistry::Impl {
  Visitor<void(void*, InspectContext)> inspector;
};

InspectorRegistry::InspectorRegistry() : pImpl{new Impl} {}
InspectorRegistry::~InspectorRegistry() { delete pImpl; }

void InspectorRegistry::RegisterCmpt(
    TypeID type, std::function<void(void*, InspectContext)> cmptInspectFunc) {
  pImpl->inspector.Register(type.GetValue(), std::move(cmptInspectFunc));
}

void InspectorRegistry::RegisterAsset(
    Type type, std::function<void(void*, InspectContext)> assetInspectFunc) {
  pImpl->inspector.Register(type.GetID().GetValue(),
                            std::move(assetInspectFunc));
}

bool InspectorRegistry::IsRegisteredCmpt(TypeID type) const {
  return pImpl->inspector.IsRegistered(type.GetValue());
}

bool InspectorRegistry::IsRegisteredAsset(Type type) const {
  return pImpl->inspector.IsRegistered(type.GetID().GetValue());
}

void InspectorRegistry::Inspect(const MyECS::World* world,
                                MyECS::CmptPtr cmpt) {
  pImpl->inspector.Visit(cmpt.Type().GetValue(), cmpt.Ptr(),
                         InspectContext{world, pImpl->inspector});
}

void InspectorRegistry::Inspect(Type type, void* asset) {
  pImpl->inspector.Visit(type.GetID().GetValue(), asset,
                         InspectContext{nullptr, pImpl->inspector});
}
