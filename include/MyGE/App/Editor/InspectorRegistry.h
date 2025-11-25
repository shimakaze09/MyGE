#pragma once

#include <MyVisitor/ncVisitor.h>

#include <MyDRefl/MyDRefl.hpp>
#include <MyECS/MyECS.hpp>
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

// Description: Make a float or int variable be restricted to a specific minimum
// value. (conflict with range) Value      : float or int
static constexpr char min_value[] = "MyInspector_min_value";

// Description: Make a float or int variable dragged by a specific step.
// (conflict with range) Value      : float or int
static constexpr char step[] = "MyInspector_step";

// Description: Make a float or int variable be restricted to a specific range.
// Value      : std::pair<T, T> (T is float or int)
static constexpr char range[] = "MyInspector_range";
}  // namespace MyInspector

namespace My::MyGE {
class InspectorRegistry {
 public:
  struct Playload {
    struct AssetHandle {
      xg::Guid guid;
      std::string_view name;
    };

    static constexpr const char Asset[] = "__AssetHandle";
    static constexpr const char Entity[] = "__Entity";
  };

  static InspectorRegistry& Instance() noexcept {
    static InspectorRegistry instance;
    return instance;
  }

  struct InspectContext {
    const MyECS::World* world;
    const Visitor<void(void*, InspectContext)>& inspector;
  };

  void Register(TypeID,
                std::function<void(void*, InspectContext)> assetInspectFunc);

  template <typename Func>
  void Register(Func&& func);

  bool IsRegistered(TypeID) const;
  bool IsRegistered(Type) const;

  void InspectComponent(const MyECS::World*, MyECS::CmptPtr);
  void Inspect(const MyECS::World*, TypeID, void* obj);

  template <typename T>
    requires std::negation_v<std::is_void<std::decay_t<T>>>
  void Inspect(T* obj) {
    Inspect(Type_of<T>, obj);
  }

  static void InspectRecursively(std::string_view name, TypeID, void* obj,
                                 InspectContext ctx);
  static void InspectRecursively(std::string_view name, MyDRefl::ObjectView obj,
                                 InspectContext ctx);

 private:
  InspectorRegistry();
  ~InspectorRegistry();
  struct Impl;
  Impl* pImpl;
};
}  // namespace My::MyGE

#include "details/InspectorRegistry.inl"
