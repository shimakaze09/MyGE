//
// Created by Admin on 15/03/2025.
//

#pragma once

#include <MyECS/Entity.h>

#include <filesystem>
#include <unordered_map>
#include <vector>

#include "../_deps/crossguid/guid.hpp"

namespace My::MyGE {
// ref: https://docs.unity3d.com/ScriptReference/AssetDatabase.html
// asset : a file in the assets folder
class AssetMngr {
 public:
  static AssetMngr& Instance() {
    static AssetMngr instance;
    return instance;
  }

  void Clear();

  // Get the GUID for the asset at path.
  // If the asset does not exist, AssetPathToGUID will return invalid xg::Guid
  xg::Guid AssetPathToGUID(const std::filesystem::path& path) const;

  void CreateAsset(const void* ptr, const std::filesystem::path& path);

  bool Contains(const void* ptr) const;

  // if ptr is not an asset, return empty path
  const std::filesystem::path& GetAssetPath(const void* ptr) const;

  // gets the corresponding asset path for the supplied guid, or an empty path
  // if the GUID can't be found.
  const std::filesystem::path& GUIDToAssetPath(const xg::Guid& guid) const;

  // import asset at path (relative)
  // * generate meta
  // * gen raw format in cache folder
  // support : .lua
  void ImportAsset(const std::filesystem::path& path);

  // load first asset at path
  void* LoadAsset(const std::filesystem::path& path);
  // returns the first asset object of type at given path
  void* LoadAsset(const std::filesystem::path& path, const std::type_info&);
  template <typename T>
  T* LoadAsset(const std::filesystem::path& path);

 private:
  struct Impl;
  Impl* pImpl;
  AssetMngr();
  ~AssetMngr();
};
}  // namespace My::MyGE

#include "detail/AssetMngr.inl"
