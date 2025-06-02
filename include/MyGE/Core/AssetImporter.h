#pragma once

#include <MyDRefl/MyDRefl.hpp>
#include <_deps/crossguid/guid.hpp>
#include <map>

#include "Serializer.h"

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

/*
 * [Template]
 * class XXXImporter final : public TAssetImporter<XXXImporter> {
 * public:
 *   using TAssetImporter<XXXImporter>::TAssetImporter;
 *   virtual AssetImportContext ImportAsset() const override {
 *     AssetImportContext ctx;
 *     const auto& path = GetFullPath();
 *     if (path.empty()) return {};
 *     // fill ctx with path ...
 *     return ctx;
 *   }
 *  static void RegisterToMyDRefl() {
 *    RegisterToMyDReflHelper();
 *    // Register fields
 *    // Register Asset XXX
 *  }
 *
 *  // fields
 * };
 */
class AssetImporter {
 public:
  AssetImporter() = default;
  AssetImporter(xg::Guid guid) : guid{guid} {}
  virtual ~AssetImporter() = default;

  const xg::Guid& GetGuid() const noexcept { return guid; }
  std::filesystem::path GetFullPath() const;

  // [Tempalte]
  // virtual MyDRefl::ObjectView This() const noexcept override
  // { return TmplThis<Impl>(this); }
  virtual MyDRefl::ObjectView This() const noexcept = 0;

  // serialize self (importer) to ctx
  // [Template]
  // {
  //   "__TypeID":<uint64>,
  //   "__TypeName":<string>,
  //   "__Content":{
  //     "__Guid":<string>
  //     ...
  //   }
  // }
  virtual void Serialize(Serializer::SerializeContext& ctx) const {
    Serializer::SerializeRecursion(This(), ctx);
  }

  // default: use Serilizer to serialize the main asset
  virtual std::string ReserializeAsset() const;

  // default: use Serilizer to deserialize file at path (<=> guid)
  virtual AssetImportContext ImportAsset() const;

 protected:
  template <typename Impl>
  static MyDRefl::ObjectView TmplThis(const Impl* ptr) {
    return {Type_of<Impl>, const_cast<Impl*>(ptr)};
  }

 private:
  friend class AssetMngr;
  static void RegisterToMyDRefl();  // call by AssetMngr

  xg::Guid guid;
};

template <typename Impl>
class TAssetImporter : public AssetImporter {
 public:
  using AssetImporter::AssetImporter;

  virtual MyDRefl::ObjectView This() const noexcept override final {
    return TmplThis(static_cast<const Impl*>(this));
  }

 protected:
  static void RegisterToMyDReflHelper() {
    MyDRefl::Mngr.RegisterType<Impl>();
    MyDRefl::Mngr.AddBases<Impl, AssetImporter>();
  }
};

/*
 * [Template]
 * class XXXImporterCreator final : public TAssetImporterCreator<XXXImporter> {
 *   virtual std::vector<std::string> SupportedExtentions() const override {
 *     return { ".abc", ".def", ... };
 *   }
 * };
 */
class AssetImporterCreator {
 public:
  virtual ~AssetImporterCreator() = default;

  // the guid is not registered into asset mngr yet, but we can store it in
  // assetimporter
  virtual std::shared_ptr<AssetImporter> CreateAssetImporter(xg::Guid guid) = 0;

  // reserialize
  virtual std::shared_ptr<AssetImporter> DeserializeAssetImporter(
      std::string_view json) {
    auto importer_impl = Serializer::Instance().Deserialize(json);
    auto importer_base =
        importer_impl.StaticCast_DerivedToBase(Type_of<AssetImporter>);
    if (!importer_base) return {};
    auto importer = importer_base.AsShared<AssetImporter>();
    return importer;
  }

  virtual std::vector<std::string> SupportedExtentions() const = 0;
};

template <typename Importer>
class TAssetImporterCreator : public AssetImporterCreator {
 public:
  // the guid is not registered into asset mngr yet, but we can store it in
  // assetimporter
  virtual std::shared_ptr<AssetImporter> CreateAssetImporter(
      xg::Guid guid) override final {
    OnceRegisterAssetImporterToMyDRefl();
    return do_CreateAssetImporter(guid);
  }

  // reserialize
  virtual std::shared_ptr<AssetImporter> DeserializeAssetImporter(
      std::string_view json) override final {
    OnceRegisterAssetImporterToMyDRefl();
    return do_DeserializeAssetImporter(json);
  }

 protected:
  virtual std::shared_ptr<AssetImporter> do_CreateAssetImporter(xg::Guid guid) {
    return std::make_shared<Importer>(guid);
  }
  virtual std::shared_ptr<AssetImporter> do_DeserializeAssetImporter(
      std::string_view json) {
    return AssetImporterCreator::DeserializeAssetImporter(json);
  }

 private:
  void OnceRegisterAssetImporterToMyDRefl() {
    static bool init = false;
    if (init) return;
    Importer::RegisterToMyDRefl();
    init = true;
  }
};

struct DefaultAsset {};

class DefaultAssetImporter final : public TAssetImporter<DefaultAssetImporter> {
 public:
  using TAssetImporter<DefaultAssetImporter>::TAssetImporter;
  virtual AssetImportContext ImportAsset() const override;

 private:
  friend class TAssetImporterCreator<DefaultAssetImporter>;
  static void RegisterToMyDRefl();
};

class DefaultAssetImporterCreator final
    : public TAssetImporterCreator<DefaultAssetImporter> {
  virtual std::vector<std::string> SupportedExtentions() const override {
    return {};
  }
};
}  // namespace Smkz::MyGE
