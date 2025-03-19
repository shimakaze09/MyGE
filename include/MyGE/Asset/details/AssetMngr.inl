//
// Created by Admin on 15/03/2025.
//

#pragma once

namespace My::MyGE {
template <typename T>
T* AssetMngr::LoadAsset(const std::filesystem::path& path) {
  void* ptr = LoadAsset(path, typeid(std::decay_t<T>));
  return reinterpret_cast<T*>(ptr);
}

template <typename T>
T* AssetMngr::GUIDToAsset(const xg::Guid& guid) const {
  void* ptr = GUIDToAsset(guid, typeid(std::decay_t<T>));
  return reinterpret_cast<T*>(ptr);
}
}  // namespace My::MyGE

inline bool operator<(const xg::Guid& lhs, const xg::Guid& rhs) noexcept {
  static_assert(sizeof(xg::Guid) % sizeof(size_t) == 0);
  auto lhs_p = reinterpret_cast<const size_t*>(&lhs.bytes());
  auto rhs_p = reinterpret_cast<const size_t*>(&rhs.bytes());
  for (size_t i = 0; i < sizeof(xg::Guid) / sizeof(size_t); i++) {
    size_t lhs_vi = *(lhs_p + i);
    size_t rhs_vi = *(rhs_p + i);
    if (lhs_vi < rhs_vi)
      return true;
    if (lhs_vi > rhs_vi)
      return false;
  }
  return false;
}
