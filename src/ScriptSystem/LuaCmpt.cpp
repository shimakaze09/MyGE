//
// Created by Admin on 14/03/2025.
//

#include "LuaCmpt.h"

#include <MyECS/World.h>

using namespace My::DustEngine;

void LuaCmpt::SetZero() {
  assert(ptr.Type().GetAccessMode() == MyECS::AccessMode::WRITE);
  size_t size = MyECS::RTDCmptTraits::Instance().Sizeof(ptr.Type());
  memset(ptr.Ptr(), 0, size);
}

void LuaCmpt::MemCpy(void* src) {
  assert(ptr.Type().GetAccessMode() == MyECS::AccessMode::WRITE);
  size_t size = MyECS::RTDCmptTraits::Instance().Sizeof(ptr.Type());
  memcpy(ptr.Ptr(), src, size);
}

void LuaCmpt::MemCpy(void* src, size_t offset, size_t size) {
  assert(ptr.Type().GetAccessMode() == MyECS::AccessMode::WRITE);
  memcpy(Offset(offset), src, size);
}
