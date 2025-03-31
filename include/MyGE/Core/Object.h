#pragma once

#include <atomic>

namespace My::MyGE {
class Object {
 public:
  Object() noexcept : id{curID++} {}

  Object(const Object&) noexcept : id{curID++} {}

  Object(Object&& rhs) noexcept : id{rhs.id} {
    rhs.id = static_cast<size_t>(-1);
  }

  Object& operator=(const Object&) noexcept { return *this; }

  Object& operator=(Object&& rhs) noexcept {
    id = rhs.id;
    rhs.id = static_cast<size_t>(-1);
    return *this;
  }

  virtual ~Object() noexcept = default;

  size_t GetInstanceID() const noexcept { return id; }

 private:
  size_t id;
  inline static std::atomic<size_t> curID{0};
};
}  // namespace My::MyGE

#include "details/Object_AutoRefl.inl"
