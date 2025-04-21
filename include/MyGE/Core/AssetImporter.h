#pragma once

#include <MyDRefl/MyDRefl.hpp>
#include <_deps/crossguid/guid.hpp>
#include <map>

namespace Smkz::MyGE {
class AssetImportContext {
 public:
  void AddObject(std::string id, MyDRefl::SharedObject obj) {
    assets.insert_or_assign(std::move(id), obj);
  }
  void SetMainObjectID(std::string id) { mainObjID = std::move(id); }
  const std::map<std::string, MyDRefl::SharedObject>& GetAssets()
      const noexcept {
    return assets;
  }

  const std::string& GetMainObjectID() const noexcept { return mainObjID; }
  MyDRefl::SharedObject GetMainObject() const noexcept {
    auto target = assets.find(mainObjID);
    if (target == assets.end())
      return {};
    else
      return target->second;
  }

 private:
  std::string mainObjID;
  std::map<std::string, MyDRefl::SharedObject> assets;
};

class AssetImporter {
 public:
  virtual ~AssetImporter() = default;

  const xg::Guid& GetGuid() const noexcept { return guid; }

  virtual void Serialize() const = 0;
  virtual bool ReserializeAsset() const { return false; }
  virtual AssetImportContext ImportAsset() const = 0;

 protected:
  xg::Guid guid;  // you must set it
};

class AssetImporterCreator {
 public:
  virtual ~AssetImporterCreator() = default;
  virtual std::shared_ptr<AssetImporter> CreateAssetImporter(xg::Guid guid) = 0;
};
}  // namespace Smkz::MyGE
