#pragma once

#include <MyContainer/Span.h>
#include <vector>

namespace My::MyGE {
template <typename T>
class LuaArray {
 public:
  void PushBack(T val) { elems.push_back(val); }

  T* Data() { return elems.data(); }

  size_t Size() const { return elems.size(); }

  Span<const T> ToConstSpan() const { return elems; }

 private:
  std::vector<T> elems;
};
}  // namespace My::MyGE

#include "detail/LuaArray_AutoRefl.inl"
