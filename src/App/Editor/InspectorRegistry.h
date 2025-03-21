#pragma once

#include <MyDP/Visitor/ncVisitor.h>
#include <MyECS/World.h>

#include <_deps/crossguid/guid.hpp>
#include <functional>

namespace My::MyGE {
class InspectorRegistry {
 public:
  static InspectorRegistry& Instance() noexcept {
    static InspectorRegistry instance;
    return instance;
  }

  struct InspectContext {
    const MyECS::World* world;
    const Visitor<void(void*, InspectContext)>& inspector;
  };

  void RegisterCmpt(MyECS::CmptType,
                    std::function<void(void*, InspectContext)> cmptInspectFunc);

  template <typename Func>
  void RegisterCmpt(Func&& func);

  template <typename... Cmpts>
  void RegisterCmpts();

  void RegisterAsset(
      const std::type_info&,
      std::function<void(void*, InspectContext)> assetInspectFunc);

  template <typename Func>
  void RegisterAsset(Func&& func);

  template <typename... Assets>
  void RegisterAssets();

  bool IsRegisteredCmpt(MyECS::CmptType) const;
  bool IsRegisteredAsset(const std::type_info&) const;

  void Inspect(const MyECS::World*, MyECS::CmptPtr);
  void Inspect(const std::type_info&, void* asset);

 private:
  InspectorRegistry();
  ~InspectorRegistry();
  struct Impl;
  Impl* pImpl;
};
}  // namespace My::MyGE

#include "details/InspectorRegistry.inl"
