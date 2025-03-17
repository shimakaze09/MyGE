//
// Created by Admin on 17/03/2025.
//

#include <MyECS/IListener.h>
#include <MyECS/World.h>
#include <MyGE/Asset/Serializer.h>
#include <MySRefl/MySRefl.h>
#include <rapidjson/error/en.h>

#include <iostream>

using namespace My::MyGE;
using namespace My::MyECS;
using namespace My;
using namespace rapidjson;
using namespace std;

struct Serializer::Impl : IListener {
  StringBuffer sb;
  Writer<StringBuffer> writer;

  Visitor<void(const void*, Serializer::SerializeContext)> serializer;
  Visitor<void(void*, const rapidjson::Value&, Serializer::DeserializeContext)>
      deserializer;

  Impl() { writer.Reset(sb); }

  void Clear() {
    sb.Clear();
    writer.Reset(sb);
  }

  virtual void EnterWorld(const World* world) override {
    writer.StartObject();
    writer.Key("entityMngr");
  }
  virtual void ExistWorld(const World* world) override { writer.EndObject(); }

  virtual void EnterSystemMngr(const SystemMngr*) override {}
  virtual void ExistSystemMngr(const SystemMngr*) override {}

  virtual void EnterSystem(const System*) override {}
  virtual void ExistSystem(const System*) override {}

  virtual void EnterEntityMngr(const EntityMngr*) override {
    writer.StartObject();
    writer.Key("entities");
    writer.StartArray();
  }
  virtual void ExistEntityMngr(const EntityMngr*) override {
    writer.EndArray();  // entities
    writer.EndObject();
  }

  virtual void EnterEntity(const Entity* e) override {
    writer.StartObject();
    writer.Key("index");
    writer.Uint64(e->Idx());
    writer.Key("components");
    writer.StartArray();
  }
  virtual void ExistEntity(const Entity*) override {
    writer.EndArray();  // components
    writer.EndObject();
  }

  virtual void EnterCmptPtr(const CmptPtr* p) override {
    writer.StartObject();
    writer.Key("type");
    writer.Uint64(p->Type().HashCode());
    if (serializer.IsRegistered(p->Type().HashCode()))
      serializer.Visit(p->Type().HashCode(), p->Ptr(),
                       Serializer::SerializeContext{&writer, &serializer});
  }
  virtual void ExistCmptPtr(const CmptPtr*) override { writer.EndObject(); }
};

Serializer::Serializer() : pImpl{new Impl} {}

Serializer::~Serializer() { delete pImpl; }

string Serializer::ToJSON(const World* world) {
  world->Accept(pImpl);
  auto json = pImpl->sb.GetString();
  pImpl->Clear();
  return json;
}

void Serializer::ToWorld(MyECS::World* world, string_view json) {
  Document doc;
  ParseResult rst = doc.Parse(json.data());

  if (!rst) {
    cerr << "ERROR::DeserializerJSON::DeserializeScene:" << endl
         << "\t" << "JSON parse error: " << GetParseError_En(rst.Code()) << " ("
         << rst.Offset() << ")" << endl;
    return;
  }

  auto entityMngr = doc["entityMngr"].GetObject();
  auto entities = entityMngr["entities"].GetArray();

  // 1. use free entry
  // 2. use new entry
  EntityIdxMap entityIdxMap;

  const auto& freeEntries = world->entityMngr.GetEntityFreeEntries();
  size_t leftFreeEntryNum = freeEntries.size();
  size_t newEntityIndex = world->entityMngr.TotalEntityNum() + leftFreeEntryNum;
  for (const auto& val_e : entities) {
    const auto& e = val_e.GetObject();
    size_t index = e["index"].GetUint64();
    if (leftFreeEntryNum > 0) {
      size_t freeIdx = freeEntries[--leftFreeEntryNum];
      size_t version = world->entityMngr.GetEntityVersion(freeIdx);
      entityIdxMap.emplace(index, Entity{freeIdx, version});
    } else
      entityIdxMap.emplace(index, Entity{newEntityIndex++, 0});
  }

  for (const auto& val_e : entities) {
    const auto& jsonEntity = val_e.GetObject();
    const auto& components = jsonEntity["components"].GetArray();

    std::vector<CmptType> cmptTypes;
    for (const auto& val_cmpt : components) {
      const auto& cmpt = val_cmpt.GetObject();
      size_t cmptID = cmpt["type"].GetUint64();
      auto type = CmptType{cmptID};
      cmptTypes.push_back(type);
    }

    auto entity = world->entityMngr.Create(cmptTypes.data(), cmptTypes.size());
    auto cmpts = world->entityMngr.Components(entity);
    for (size_t i = 0; i < cmpts.size(); i++) {
      if (pImpl->deserializer.IsRegistered(cmpts[i].Type().HashCode())) {
        pImpl->deserializer.Visit(
            cmpts[i].Type().HashCode(), cmpts[i].Ptr(),
            components[static_cast<SizeType>(i)],
            DeserializeContext{&entityIdxMap, &(pImpl->deserializer)});
      }
    }
  }
}

void Serializer::RegisterComponentSerializeFunction(MyECS::CmptType type,
                                                    SerializeFunc func) {
  RegisterUserTypeSerializeFunction(type.HashCode(), std::move(func));
}

void Serializer::RegisterComponentDeserializeFunction(MyECS::CmptType type,
                                                      DeserializeFunc func) {
  RegisterUserTypeDeserializeFunction(type.HashCode(), std::move(func));
}

void Serializer::RegisterUserTypeSerializeFunction(size_t id,
                                                   SerializeFunc func) {
  pImpl->serializer.Register(id, std::move(func));
}

void Serializer::RegisterUserTypeDeserializeFunction(size_t id,
                                                     DeserializeFunc func) {
  pImpl->deserializer.Register(id, std::move(func));
}
