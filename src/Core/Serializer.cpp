#include <MyGE/Core/AssetMngr.h>
#include <MyGE/Core/Serializer.h>
#include <rapidjson/error/en.h>

#include <MyDRefl/MyDRefl.hpp>
#include <MyECS/IListener.hpp>
#include <MyECS/MyECS.hpp>
#include <_deps/crossguid/guid.hpp>
#include <iostream>

using namespace Smkz::MyGE;
using namespace Smkz::MyECS;
using namespace Smkz::MyDRefl;
using namespace Smkz;
using namespace rapidjson;
using namespace std;

namespace Smkz::MyGE::details {
static bool Traits_BeginEnd(Type t) {
  bool contain_begin = false, contain_end = false;
  for (const auto& [name, info] : MethodRange(t)) {
    if (name == NameIDRegistry::Meta::container_begin) contain_begin = true;
    if (name == NameIDRegistry::Meta::container_end) contain_end = true;
    if (contain_begin && contain_end) return true;
  }

  return false;
}
enum class AddMode { PushBack, PushFront, Insert, Push, None };
static AddMode Traits_AddMode(Type t) {
  for (const auto& [name, info] : MethodRange(t)) {
    if (name == NameIDRegistry::Meta::container_push_back)
      return AddMode::PushBack;
    else if (name == NameIDRegistry::Meta::container_push_front)
      return AddMode::PushFront;
    else if (name == NameIDRegistry::Meta::container_insert)
      return AddMode::Insert;
    else if (name == NameIDRegistry::Meta::container_push)
      return AddMode::Push;
  }
  return AddMode::None;
}
}  // namespace Smkz::MyGE::details

struct Serializer::Impl {
  Visitor<void(const void*, SerializeContext&)> serializer;
  Visitor<void(void*, const rapidjson::Value&, DeserializeContext&)>
      deserializer;

  struct WorldSerializer : IListener {
    SerializeContext ctx;
    const World* w;

    WorldSerializer(
        const Visitor<void(const void*, SerializeContext&)>& serializer)
        : ctx{serializer}, w{nullptr} {
      ctx.writer.Reset(ctx.sb);
    }

    virtual void EnterWorld(const World* world) override {
      ctx.writer.StartObject();
      ctx.writer.Key(Serializer::Key::EntityMngr);
      w = world;
    }
    virtual void ExistWorld(const World* world) override {
      ctx.writer.EndObject();
      w = nullptr;
    }

    virtual void EnterEntityMngr(const EntityMngr*) override {
      ctx.writer.StartObject();
      ctx.writer.Key(Serializer::Key::Entities);
      ctx.writer.StartArray();
    }
    virtual void ExistEntityMngr(const EntityMngr*) override {
      ctx.writer.EndArray();  // entities
      ctx.writer.EndObject();
    }

    virtual void EnterEntity(Entity e) override {
      ctx.writer.StartObject();
      ctx.writer.Key(Key::Index);
      ctx.writer.Uint64(e.index);
      ctx.writer.Key(Key::Components);
      ctx.writer.StartArray();
    }
    virtual void ExistEntity(Entity) override {
      ctx.writer.EndArray();  // components
      ctx.writer.EndObject();
    }

    virtual void EnterCmpt(CmptPtr p) override {
      Serializer::SerializeRecursion({Mngr.tregistry.Typeof(p.Type()), p.Ptr()},
                                     ctx);
    }
    virtual void ExistCmpt(CmptPtr) override {
      // do nothing
    }
  };
};

void Serializer::SerializeRecursion(MyDRefl::ObjectView obj,
                                    SerializeContext& ctx) {
  if (obj.GetType().IsReference()) {
    ctx.writer.String(Key::NotSupport);
    return;
  }

  obj = obj.RemoveConst();
  if (obj.GetType().IsArithmetic()) {
    switch (obj.GetType().GetID().GetValue()) {
      case TypeID_of<bool>.GetValue():
        ctx.writer.Bool(obj.As<bool>());
        break;
      case TypeID_of<std::int8_t>.GetValue():
        ctx.writer.Int(obj.As<std::int8_t>());
        break;
      case TypeID_of<std::int16_t>.GetValue():
        ctx.writer.Int(obj.As<std::int16_t>());
        break;
      case TypeID_of<std::int32_t>.GetValue():
        ctx.writer.Int(obj.As<std::int32_t>());
        break;
      case TypeID_of<std::int64_t>.GetValue():
        ctx.writer.Int64(obj.As<std::int64_t>());
        break;
      case TypeID_of<std::uint8_t>.GetValue():
        ctx.writer.Uint(obj.As<std::uint8_t>());
        break;
      case TypeID_of<std::uint16_t>.GetValue():
        ctx.writer.Uint(obj.As<std::uint16_t>());
        break;
      case TypeID_of<std::uint32_t>.GetValue():
        ctx.writer.Uint(obj.As<std::uint32_t>());
        break;
      case TypeID_of<std::uint64_t>.GetValue():
        ctx.writer.Uint64(obj.As<std::uint64_t>());
        break;
      case TypeID_of<float>.GetValue():
        ctx.writer.Double(obj.As<float>());
        break;
      case TypeID_of<double>.GetValue():
        ctx.writer.Double(obj.As<double>());
        break;
      default:
        assert(false);
    }
    return;
  } else if (obj.GetType().Is<std::string>()) {
    ctx.writer.String(obj.As<std::string>());
    return;
  } else if (obj.GetType().Is<std::pmr::string>()) {
    ctx.writer.String(obj.As<std::pmr::string>().data());
    return;
  } else if (obj.GetType().Is<xg::Guid>()) {
    ctx.writer.String(obj.As<xg::Guid>().str());
    return;
  }

  if (ctx.serializer.IsRegistered(obj.GetType().GetID().GetValue())) {
    ctx.serializer.Visit(obj.GetType().GetID().GetValue(), obj.GetPtr(), ctx);
    return;
  }

  ctx.writer.StartObject();
  ctx.writer.Key(Key::TypeID);
  ctx.writer.Uint64(obj.GetType().GetID().GetValue());
  ctx.writer.Key(Key::TypeName);
  ctx.writer.String(
      obj.GetType().GetName().data(),
      static_cast<rapidjson::SizeType>(obj.GetType().GetName().size()));
  ctx.writer.Key(Key::Content);

  // write content

  if (ctx.serializer.IsRegistered(obj.GetType().GetID().GetValue()))
    ctx.serializer.Visit(obj.GetType().GetID().GetValue(), obj.GetPtr(), ctx);
  else if (obj.GetType().IsEnum()) {
    bool found = false;
    for (const auto& [name, v] : obj.GetVars(FieldFlag::Unowned)) {
      if (v == obj) {
        ctx.writer.String(name.GetView().data());
        found = true;
        break;
      }
    }
    if (!found) ctx.writer.String(Key::NotSupport);
  } else if (obj.GetType().Is<MyECS::Entity>())
    ctx.writer.Uint64(obj.As<Entity>().index);
  else if (obj.GetType().Is<SharedObject>()) {
    auto sobj = obj.As<SharedObject>();
    if (AssetMngr::Instance().Contains(sobj)) {
      ctx.writer.StartObject();
      ctx.writer.Key(Key::Name);
      ctx.writer.String(AssetMngr::Instance().NameofAsset(sobj).data());
      ctx.writer.Key(Key::Guid);
      ctx.writer.String(AssetMngr::Instance().GetAssetGUID(sobj).str());
      ctx.writer.EndObject();
    } else
      ctx.writer.String(Key::NotSupport);
  } else if (obj.GetType().GetName().starts_with(
                 "Smkz::MyGE::SharedVar<")) {  // TODO
    auto sobj = obj.Invoke("cast_to_shared_obj");
    if (AssetMngr::Instance().Contains(sobj)) {
      ctx.writer.StartObject();
      ctx.writer.Key(Key::Name);
      ctx.writer.String(AssetMngr::Instance().NameofAsset(sobj).data());
      ctx.writer.Key(Key::Guid);
      ctx.writer.String(AssetMngr::Instance().GetAssetGUID(sobj).str());
      ctx.writer.EndObject();
    } else
      ctx.writer.String(Key::NotSupport);
  } else if (auto attr =
                 Mngr.GetTypeAttr(obj.GetType(), Type_of<ContainerType>);
             attr.GetType().Valid()) {
    ContainerType ct = attr.As<ContainerType>();
    switch (ct) {
      case Smkz::MyDRefl::ContainerType::Span:
      case Smkz::MyDRefl::ContainerType::Stack:
      case Smkz::MyDRefl::ContainerType::Queue:
      case Smkz::MyDRefl::ContainerType::PriorityQueue:
      case Smkz::MyDRefl::ContainerType::None:
        ctx.writer.String(Key::NotSupport);
        break;
      case Smkz::MyDRefl::ContainerType::Array:
      case Smkz::MyDRefl::ContainerType::Deque:
      case Smkz::MyDRefl::ContainerType::ForwardList:
      case Smkz::MyDRefl::ContainerType::List:
      case Smkz::MyDRefl::ContainerType::MultiSet:
      case Smkz::MyDRefl::ContainerType::Map:
      case Smkz::MyDRefl::ContainerType::MultiMap:
      case Smkz::MyDRefl::ContainerType::RawArray:
      case Smkz::MyDRefl::ContainerType::Set:
      case Smkz::MyDRefl::ContainerType::UnorderedMap:
      case Smkz::MyDRefl::ContainerType::UnorderedMultiSet:
      case Smkz::MyDRefl::ContainerType::UnorderedMultiMap:
      case Smkz::MyDRefl::ContainerType::UnorderedSet:
      case Smkz::MyDRefl::ContainerType::Vector:
        ctx.writer.StartArray();
        {
          auto e = obj.end();
          for (auto iter = obj.begin(); iter != e; ++iter)
            SerializeRecursion((*iter).RemoveReference(), ctx);
        }
        ctx.writer.EndArray();
        break;
      case Smkz::MyDRefl::ContainerType::Pair:
      case Smkz::MyDRefl::ContainerType::Tuple:
        ctx.writer.StartArray();
        {
          std::size_t size = obj.tuple_size();
          for (std::size_t i = 0; i < size; i++)
            SerializeRecursion(obj.get(i).RemoveReference(), ctx);
        }
        ctx.writer.EndArray();
        break;
      case Smkz::MyDRefl::ContainerType::Variant:
        SerializeRecursion(obj.variant_visit_get().RemoveReference(), ctx);
        break;
      case Smkz::MyDRefl::ContainerType::Optional:
        if (obj.has_value())
          SerializeRecursion(obj.value().RemoveReference(), ctx);
        else
          ctx.writer.Null();
        break;
      default:
        assert(false);
        break;
    }
  } else if (details::Traits_BeginEnd(obj.GetType())) {
    ctx.writer.StartArray();
    {
      auto e = obj.end();
      for (auto iter = obj.begin(); iter != e; ++iter)
        SerializeRecursion((*iter).RemoveReference(), ctx);
    }
    ctx.writer.EndArray();
  } else {
    ctx.writer.StartObject();
    for (const auto& [n, obj] : obj.GetVars(FieldFlag::Owned)) {
      ctx.writer.Key(n.GetView().data());
      SerializeRecursion(obj, ctx);
    }
    ctx.writer.EndObject();
  }

  ctx.writer.EndObject();
}

MyDRefl::SharedObject Serializer::DeserializeRecursion(
    const rapidjson::Value& value, Serializer::DeserializeContext& ctx) {
  if (value.IsBool())
    return Mngr.MakeShared(Type_of<bool>, TempArgsView{value.GetBool()});
  if (value.IsDouble())
    return Mngr.MakeShared(Type_of<double>, TempArgsView{value.GetDouble()});
  if (value.IsFloat())
    return Mngr.MakeShared(Type_of<float>, TempArgsView{value.GetFloat()});
  if (value.IsInt())
    return Mngr.MakeShared(Type_of<int>, TempArgsView{value.GetInt()});
  if (value.IsInt64())
    return Mngr.MakeShared(Type_of<std::int64_t>,
                           TempArgsView{value.GetInt64()});
  if (value.IsUint())
    return Mngr.MakeShared(Type_of<unsigned int>,
                           TempArgsView{value.GetUint()});
  if (value.IsUint64())
    return Mngr.MakeShared(Type_of<std::uint64_t>,
                           TempArgsView{value.GetUint64()});
  if (value.IsString())
    return Mngr.MakeShared(Type_of<std::string>,
                           TempArgsView{value.GetString()});
  if (value.IsNull()) return Mngr.MakeShared(Type_of<std::nullptr_t>);

  assert(value.IsObject());
  const auto& jsonObj = value.GetObject();
  if (jsonObj.FindMember(Serializer::Key::TypeID) == jsonObj.end()) return {};

  std::uint64_t id = jsonObj[Serializer::Key::TypeID].GetUint64();
  Type type = Mngr.tregistry.Typeof(TypeID{id});

  if (type.IsReference()) {
    assert(false);  // not support
    return {};
  }

  type = type.RemoveConst();

  const rapidjson::Value& content = jsonObj[Serializer::Key::Content];

  // content -> obj

  if (ctx.deserializer.IsRegistered(type.GetID().GetValue())) {
    auto* info = Mngr.GetTypeInfo(type);
    void* buffer = Mngr.GetObjectResource()->allocate(
        std::max<std::size_t>(1, info->size), info->alignment);
    ctx.deserializer.Visit(type.GetID().GetValue(), buffer, content, ctx);
    return SharedObject(type,
                        SharedBuffer(buffer, [type, s = info->size,
                                              a = info->alignment](void* ptr) {
                          Mngr.Destruct({type, ptr});
                          Mngr.GetObjectResource()->deallocate(ptr, s, a);
                        }));
  } else if (type.Is<SharedObject>()) {
    if (!content.IsObject()) return {};  // not support

    auto asset = content.GetObject();
    auto n = asset[Key::Name].GetString();
    auto guid = xg::Guid{asset[Key::Guid].GetString()};

    auto obj = AssetMngr::Instance().GUIDToAsset(guid, n);
    // SharedObject of SharedObject
    return Mngr.MakeShared(
        Type_of<SharedObject>,
        TempArgsView{ObjectView{Type_of<SharedObject>, &obj}});
  } else if (type.GetName().starts_with("Smkz::MyGE::SharedVar<")) {  // TODO
    if (!content.IsObject()) return {};  // not support

    auto asset = content.GetObject();
    auto n = asset[Key::Name].GetString();
    auto guid = xg::Guid{asset[Key::Guid].GetString()};

    auto obj = AssetMngr::Instance().GUIDToAsset(guid, n);
    // SharedVar of SharedObject
    return Mngr.MakeShared(
        type, TempArgsView{ObjectView{Type_of<SharedObject>, &obj}});
  } else if (type.IsEnum()) {
    std::string name = content.GetString();
    if (name == Key::NotSupport) return Mngr.MakeShared(type);

    for (const auto& [n, v] : VarRange{type}) {
      if (n.Is(name)) return Mngr.MakeShared(type, TempArgsView{v});
    }

    return Mngr.MakeShared(type);
  } else if (type.Is<Entity>()) {
    assert(content.IsUint64());
    return Mngr.MakeShared(
        type, TempArgsView{ctx.entityIdxMap.at(content.GetUint64())});
  } else if (type.IsArithmetic()) {
    switch (type.GetID().GetValue()) {
      case TypeID_of<bool>.GetValue():
        return Mngr.MakeShared(type, TempArgsView{content.GetBool()});
      case TypeID_of<std::int8_t>.GetValue():
        return Mngr.MakeShared(
            type, TempArgsView{static_cast<std::int8_t>(content.GetInt())});
      case TypeID_of<std::int16_t>.GetValue():
        return Mngr.MakeShared(
            type, TempArgsView{static_cast<std::int16_t>(content.GetInt())});
      case TypeID_of<std::int32_t>.GetValue():
        return Mngr.MakeShared(
            type, TempArgsView{static_cast<std::int32_t>(content.GetInt())});
      case TypeID_of<std::int64_t>.GetValue():
        return Mngr.MakeShared(
            type, TempArgsView{static_cast<std::int64_t>(content.GetInt64())});
      case TypeID_of<std::uint8_t>.GetValue():
        return Mngr.MakeShared(
            type, TempArgsView{static_cast<std::uint8_t>(content.GetUint())});
      case TypeID_of<std::uint16_t>.GetValue():
        return Mngr.MakeShared(
            type, TempArgsView{static_cast<std::uint16_t>(content.GetUint())});
      case TypeID_of<std::uint32_t>.GetValue():
        return Mngr.MakeShared(
            type, TempArgsView{static_cast<std::uint32_t>(content.GetUint())});
      case TypeID_of<std::uint64_t>.GetValue():
        return Mngr.MakeShared(
            type,
            TempArgsView{static_cast<std::uint64_t>(content.GetUint64())});
      case TypeID_of<float>.GetValue():
        return Mngr.MakeShared(
            type, TempArgsView{static_cast<float>(content.GetFloat())});
      case TypeID_of<double>.GetValue():
        return Mngr.MakeShared(
            type, TempArgsView{static_cast<double>(content.GetDouble())});
      default:
        assert(false);
        return {};
    }
  } else if (auto attr = Mngr.GetTypeAttr(type, Type_of<ContainerType>);
             attr.GetType().Valid()) {
    ContainerType ct = attr.As<ContainerType>();
    switch (ct) {
      case Smkz::MyDRefl::ContainerType::Span:
      case Smkz::MyDRefl::ContainerType::Stack:
      case Smkz::MyDRefl::ContainerType::Queue:
      case Smkz::MyDRefl::ContainerType::PriorityQueue:
      case Smkz::MyDRefl::ContainerType::None:
        return {};
      case Smkz::MyDRefl::ContainerType::RawArray:
      case Smkz::MyDRefl::ContainerType::Array: {
        auto obj = Mngr.MakeShared(type);
        const auto& arr = content.GetArray();
        std::size_t N = obj.size();
        assert(N == arr.Size());
        for (std::size_t i = 0; i < N; i++) {
          obj[i].Invoke<void>(
              NameIDRegistry::Meta::operator_assignment,
              TempArgsView{DeserializeRecursion(
                  arr[static_cast<rapidjson::SizeType>(i)], ctx)},
              MethodFlag::Variable);
        }
        return obj;
      }
      case Smkz::MyDRefl::ContainerType::Deque:
      case Smkz::MyDRefl::ContainerType::Vector:
      case Smkz::MyDRefl::ContainerType::List: {
        auto obj = Mngr.MakeShared(type);
        const auto& arr = content.GetArray();
        std::size_t N = arr.Size();
        for (std::size_t i = 0; i < N; i++)
          obj.push_back(DeserializeRecursion(
              arr[static_cast<rapidjson::SizeType>(i)], ctx));
        return obj;
      }
      case Smkz::MyDRefl::ContainerType::ForwardList: {
        auto obj = Mngr.MakeShared(type);
        const auto& arr = content.GetArray();
        std::size_t N = arr.Size();
        for (std::size_t i = 0; i < N; i++)
          obj.push_front(DeserializeRecursion(
              arr[static_cast<rapidjson::SizeType>(N - 1 - i)], ctx));
        return obj;
      }
      case Smkz::MyDRefl::ContainerType::MultiSet:
      case Smkz::MyDRefl::ContainerType::MultiMap:
      case Smkz::MyDRefl::ContainerType::Set:
      case Smkz::MyDRefl::ContainerType::Map:
      case Smkz::MyDRefl::ContainerType::UnorderedMap:
      case Smkz::MyDRefl::ContainerType::UnorderedMultiSet:
      case Smkz::MyDRefl::ContainerType::UnorderedMultiMap:
      case Smkz::MyDRefl::ContainerType::UnorderedSet: {
        auto obj = Mngr.MakeShared(type);
        const auto& arr = content.GetArray();
        std::size_t N = arr.Size();
        for (std::size_t i = 0; i < N; i++)
          obj.insert(DeserializeRecursion(
              arr[static_cast<rapidjson::SizeType>(i)], ctx));
        return obj;
      }
      case Smkz::MyDRefl::ContainerType::Pair:
      case Smkz::MyDRefl::ContainerType::Tuple: {
        std::vector<SharedObject> args;
        const auto& arr = content.GetArray();
        std::size_t N = arr.Size();
        for (std::size_t i = 0; i < N; i++)
          args.push_back(DeserializeRecursion(
              arr[static_cast<rapidjson::SizeType>(i)], ctx));
        std::vector<void*> argptrs;
        std::vector<Type> argtypes;
        for (const auto& arg : args) {
          argptrs.push_back(arg.GetPtr());
          argtypes.push_back(arg.GetType());
        }
        ArgsView argsview(argptrs.data(), argtypes);
        return Mngr.MakeShared(type, argsview);
      }
      case Smkz::MyDRefl::ContainerType::Variant:
        return Mngr.MakeShared(
            type, TempArgsView{DeserializeRecursion(content, ctx)});
      case Smkz::MyDRefl::ContainerType::Optional:
        if (content.IsNull())
          return Mngr.MakeShared(type);
        else
          return Mngr.MakeShared(
              type, TempArgsView{DeserializeRecursion(content, ctx)});
      default:
        assert(false);
        return {};
    }
  } else if (auto addmode = details::Traits_AddMode(type);
             addmode != details::AddMode::None &&
             details::Traits_BeginEnd(type)) {
    auto obj = Mngr.MakeShared(type);
    const auto& arr = content.GetArray();
    std::size_t N = arr.Size();
    for (std::size_t i = 0; i < N; i++) {
      auto ele =
          DeserializeRecursion(arr[static_cast<rapidjson::SizeType>(i)], ctx);
      switch (addmode) {
        case Smkz::MyGE::details::AddMode::PushBack:
          obj.push_back(ele);
          break;
        case Smkz::MyGE::details::AddMode::PushFront:
          obj.push_front(ele);
          break;
        case Smkz::MyGE::details::AddMode::Insert:
          obj.insert(ele);
          break;
        case Smkz::MyGE::details::AddMode::Push:
          obj.push(ele);
          break;
        default:
          assert(false);
          break;
      }
    }
    return obj;
  } else {
    auto obj = Mngr.MakeShared(type);
    if (!obj.GetType()) return {};
    const auto& jsonFields = content.GetObject();
    for (const auto& [n, var] : obj.GetVars(FieldFlag::Owned)) {
      var.Invoke<void>(NameIDRegistry::Meta::operator_assignment,
                       TempArgsView{DeserializeRecursion(
                           jsonFields[n.GetView().data()], ctx)},
                       MethodFlag::Variable);
    }
    return obj;
  }
}

Serializer::Serializer() : pImpl{new Impl} {
  MyDRefl::Mngr.RegisterType<xg::Guid>();
  MyDRefl::Mngr.AddMemberMethod(
      MyDRefl::NameIDRegistry::Meta::operator_assignment,
      [](xg::Guid& obj, const std::string_view& str) { obj = xg::Guid{str}; });
  MyDRefl::Mngr.RegisterType<MyDRefl::SharedObject>();
}

Serializer::~Serializer() { delete pImpl; }

string Serializer::Serialize(const World* world) {
  Impl::WorldSerializer worldSerializer(pImpl->serializer);
  world->Accept(&worldSerializer);
  auto json = worldSerializer.ctx.sb.GetString();
  return json;
}

string Serializer::Serialize(size_t ID, const void* obj) {
  return Serialize(ObjectView{MyDRefl::Mngr.tregistry.Typeof(TypeID{ID}),
                              const_cast<void*>(obj)});
}

std::string Serializer::Serialize(ObjectView obj) {
  SerializeContext ctx{pImpl->serializer};

  SerializeRecursion(obj, ctx);
  auto json = ctx.sb.GetString();
  return json;
}

bool Serializer::SerializeToWorld(MyECS::World* world, string_view json) {
  Document doc;
  ParseResult rst = doc.Parse(json.data());

  if (!rst) {
    cerr << "ERROR::Serializer::ToWorld:" << endl
         << "\t" << "JSON parse error: " << GetParseError_En(rst.Code()) << " ("
         << rst.Offset() << ")" << endl;
    return false;
  }

  auto entityMngr = doc[Serializer::Key::EntityMngr].GetObject();
  auto entities = entityMngr[Serializer::Key::Entities].GetArray();

  // 1. use free entry
  // 2. use new entry
  EntityIdxMap entityIdxMap;
  entityIdxMap.reserve(entities.Size());

  const auto& freeEntries = world->entityMngr.GetEntityFreeEntries();
  size_t leftFreeEntryNum = freeEntries.size();
  size_t newEntityIndex = world->entityMngr.TotalEntityNum() + leftFreeEntryNum;
  for (const auto& val_e : entities) {
    const auto& e = val_e.GetObject();
    size_t index = e[Key::Index].GetUint64();
    if (leftFreeEntryNum > 0) {
      size_t freeIdx = freeEntries[--leftFreeEntryNum];
      size_t version = world->entityMngr.GetEntityVersion(freeIdx);
      entityIdxMap.emplace(index, Entity{freeIdx, version});
    } else
      entityIdxMap.emplace(index, Entity{newEntityIndex++, 0});
  }

  DeserializeContext ctx{entityIdxMap, pImpl->deserializer};

  for (const auto& val_e : entities) {
    const auto& jsonEntity = val_e.GetObject();
    const auto& jsonCmpts = jsonEntity[Key::Components].GetArray();

    std::vector<TypeID> cmptTypes;
    cmptTypes.resize(jsonCmpts.Size());
    for (SizeType i = 0; i < jsonCmpts.Size(); i++) {
      const auto& cmpt = jsonCmpts[i].GetObject();
      cmptTypes[i] = TypeID{cmpt[Key::TypeID].GetUint64()};
    }

    auto entity =
        world->entityMngr.Create(std::span{cmptTypes.data(), cmptTypes.size()});
    for (size_t i = 0; i < cmptTypes.size(); i++) {
      void* ptr = world->entityMngr.WriteComponent(entity, cmptTypes[i]).Ptr();
      ObjectView obj{MyDRefl::Mngr.tregistry.Typeof(cmptTypes[i]), ptr};
      obj.Invoke<void>(NameIDRegistry::Meta::operator_assignment,
                       TempArgsView{DeserializeRecursion(
                           jsonCmpts[static_cast<SizeType>(i)], ctx)},
                       MethodFlag::Variable);
    }
  }

  return true;
}

MyDRefl::SharedObject Serializer::Deserialize(std::string_view json) {
  Document doc;
  ParseResult rst = doc.Parse(json.data());
  EntityIdxMap emptyMap;
  DeserializeContext ctx{emptyMap, pImpl->deserializer};
  return DeserializeRecursion(doc, ctx);
}

void Serializer::RegisterSerializeFunction(TypeID id, SerializeFunc func) {
  pImpl->serializer.Register(id.GetValue(), std::move(func));
}

void Serializer::RegisterDeserializeFunction(TypeID id, DeserializeFunc func) {
  pImpl->deserializer.Register(id.GetValue(), std::move(func));
}
