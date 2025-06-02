#pragma once

#include <MyECS/Entity.hpp>
#include <memory_resource>
#include <set>

namespace Smkz::MyGE {
struct Children {
  using allocator_type = std::pmr::polymorphic_allocator<MyECS::Entity>;
  Children() = default;
  Children(const allocator_type& alloc) : value(alloc) {}
  Children(const Children& other, const allocator_type& alloc)
      : value(other.value, alloc) {}
  Children(Children&& other, const allocator_type& alloc)
      : value(std::move(other.value), alloc) {}

  std::pmr::set<MyECS::Entity> value;
};
}  // namespace Smkz::MyGE
