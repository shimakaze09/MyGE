#pragma once

#include <MyDRefl/MyDRefl.hpp>
#include <_deps/crossguid/guid.hpp>
#include <filesystem>
#include <map>
#include <memory>
#include <regex>
#include <set>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "Asset.h"
#include "AssetImporter.h"

namespace Smkz::MyGE {
// ref: https://docs.unity3d.com/ScriptReference/AssetDatabase.html
class AssetMngr {
 public:
  static AssetMngr& Instance() {
    static AssetMngr instance;
    return instance;
  }

  // default : ".."
  const std::filesystem::path& GetRootPath() const noexcept;
  void SetRootPath(std::filesystem::path path);

  void Clear();

  bool IsImported(const std::filesystem::path& path) const;

  // Get the GUID for the asset at path.
  // If the asset does not exist, AssetPathToGUID will return invalid xg::Guid
  xg::Guid AssetPathToGUID(const std::filesystem::path& path) const;

  bool CreateAsset(MyDRefl::SharedObject obj,
                   const std::filesystem::path& path);

  template <typename T>
    requires std::negation_v<std::is_void<T>>
  bool CreateAsset(std::shared_ptr<T> ptr, const std::filesystem::path& path);

  bool Contains(MyDRefl::SharedObject obj) const;

  std::vector<xg::Guid> FindAssets(const std::wregex& matchRegex) const;

  xg::Guid GetAssetGUID(MyDRefl::SharedObject obj) const;
  const std::filesystem::path& GetAssetPath(MyDRefl::SharedObject obj) const;

  // empty xg::Guid is root
  const std::map<xg::Guid, std::set<xg::Guid>>& GetAssetTree() const;

  // get first asset type
  Type GetAssetType(const std::filesystem::path&) const;

  // gets the corresponding asset path for the supplied guid, or an empty path
  // if the GUID can't be found.
  const std::filesystem::path& GUIDToAssetPath(const xg::Guid&) const;

  // if not loaded, return nullptr
  Asset GUIDToAsset(const xg::Guid&) const;
  Asset GUIDToAsset(const xg::Guid&, Type type) const;
  template <typename T>
  TAsset<T> GUIDToAsset(const xg::Guid&) const;

  // import asset at path (relative)
  // * generate meta
  xg::Guid ImportAsset(const std::filesystem::path& path);
  // recursively import asset in directory (relative)
  // not import the 'directory'
  void ImportAssetRecursively(const std::filesystem::path& directory);

  Asset LoadMainAsset(const std::filesystem::path& path);
  // returns the first asset object of type at given path
  Asset LoadAsset(const std::filesystem::path& path, Type);
  std::vector<Asset> LoadAllAssets(const std::filesystem::path& path);
  template <typename T>
  TAsset<T> LoadAsset(const std::filesystem::path& path);

  bool ReserializeAsset(const std::filesystem::path& path);

  bool MoveAsset(const std::filesystem::path& src,
                 const std::filesystem::path& dst);

  bool IsSupported(std::string_view extension) const noexcept;

  void RegisterAssetImporterCreator(
      std::string_view extension,
      std::shared_ptr<AssetImporterCreator> creator);

 private:
  struct Impl;
  Impl* pImpl;
  AssetMngr();
  ~AssetMngr();
};
}  // namespace Smkz::MyGE

#include "details/AssetMngr.inl"
