//
// Created by Admin on 16/03/2025.
//

#pragma once

#include <atomic>

namespace My::MyGE {
class Object {
 public:
  Object() : id{curID++} {}

  size_t GetInstanceID() const noexcept { return id; }

 private:
  const size_t id;
  inline static std::atomic<size_t> curID{0};
};
}  // namespace My::MyGE

#include "details/Object_AutoRefl.inl"
