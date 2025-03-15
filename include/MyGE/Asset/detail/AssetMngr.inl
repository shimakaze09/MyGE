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
}  // namespace My::MyGE
