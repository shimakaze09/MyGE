#pragma once

#include <atomic>

namespace My::MyGE {
class RenderRsrcObject {
 public:
  RenderRsrcObject() : id{curID++} {}
  size_t GetInstanceID() const noexcept { return id; }

 private:
  const size_t id;
  inline static std::atomic<size_t> curID{0};
};
}  // namespace My::MyGE

#include "details/RenderRsrcObject_AutoRefl.inl"
