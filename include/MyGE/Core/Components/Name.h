#pragma once

#include <memory_resource>
#include <string>

namespace Smkz::MyGE {
struct Name {
  using allocator_type = std::pmr::string::allocator_type;
  Name(const allocator_type& alloc) : value(alloc) {}
  Name(const Name& other, const allocator_type& alloc)
      : value(other.value, alloc) {}
  Name(Name&& other, const allocator_type& alloc)
      : value(std::move(other.value), alloc) {}

  std::pmr::string value;
};
}  // namespace Smkz::MyGE
