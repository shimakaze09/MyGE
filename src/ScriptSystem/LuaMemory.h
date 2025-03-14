//
// Created by Admin on 14/03/2025.
//

#pragma once

namespace My::MyGE {
class LuaMemory {
 public:
  static void* Malloc(size_t size);
  static void Free(void* block);
  static void* Offset(void* ptr, size_t n);
  static void Copy(void* dst, void* src, size_t size);
  static void Set(void* dst, int value, size_t size);
  static void StrCpy(char* dst, const char* src);

 private:
  LuaMemory() = delete;
};
}  // namespace My::MyGE

#include "detail/LuaMemory_AutoRefl.inl"
