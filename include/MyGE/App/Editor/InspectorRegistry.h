#pragma once

#include <MyDP/Visitor/ncVisitor.h>
#include <MyECS/World.h>

#include <_deps/crossguid/guid.hpp>
#include <functional>

namespace MyInspector {
// Description: Makes a variable not show up in the inspector but be serialized.
// Value      : void
static constexpr char hide[] = "MyInspector_hide";

// Description: Add a header above some fields,
// Value      : "..." / std::string_view
static constexpr char header[] = "MyInspector_header";

// Description: change the display name of a field or a enumerator.
// Value      : "..." / std::string_view
static constexpr char name[] = "MyInspector_name";

// Description: Specify a tooltip for a field
// Value      : "..." / std::string_view
static constexpr char tooltip[] = "MyInspector_tooltip";

// Description: Make a float or int variable be restricted to a specific minimum value. (conflict with range)
// Value      : float or int
static constexpr char min_value[] = "MyInspector_min_value";

// Description: Make a float or int variable dragged by a specific step. (conflict with range)
// Value      : float or int
static constexpr char step[] = "MyInspector_step";

// Description: Make a float or int variable be restricted to a specific range.
// Value      : std::pair<T, T> (T is float or int)
static constexpr char range[] = "MyInspector_range";
}  // namespace MyInspector

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
