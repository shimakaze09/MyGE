#pragma once

#include <MyDRefl/MyDRefl.hpp>

#include "SharedVar.h"

namespace Smkz::MyGE {
struct Asset {
  std::string name;
  MyDRefl::SharedObject obj;

  bool Valid() const noexcept { return obj.GetPtr() != nullptr; }
};

template <typename T>
struct TAsset {
  std::string name;
  SharedVar<T> obj;
  SharedVar<T>& operator->() noexcept { return obj; }
  const SharedVar<T>& operator->() const noexcept { return obj; }
  T& operator*() noexcept { return *obj; }
  const T& operator*() const noexcept { return *obj; }
  operator bool() const noexcept { return static_cast<bool>(obj); }
};
}  // namespace Smkz::MyGE
