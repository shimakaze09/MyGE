//
// Created by Admin on 17/03/2025.
//

#pragma once

#include <MyDP/Visitor/cVisitor.h>
#include <MyDP/Visitor/ncVisitor.h>
#include <MyECS/World.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>

#include <string>

namespace My::MyGE {
class Serializer {
 public:
  static Serializer& Instance() {
    static Serializer instance;
    return instance;
  }

  using JSONWriter = rapidjson::Writer<rapidjson::StringBuffer>;
  struct SerializeContext {
    JSONWriter* const writer;
    const Visitor<void(const void*, SerializeContext)>* const fieldSerializer;
  };
  using SerializeFunc = std::function<void(const void*, SerializeContext)>;
  using EntityIdxMap = std::unordered_map<size_t, MyECS::Entity>;
  struct DeserializeContext {
    const EntityIdxMap* entityIdxMap;
    const Visitor<void(void*, const rapidjson::Value&,
                       DeserializeContext)>* const fieldDeserializer;
  };
  using DeserializeFunc =
      std::function<void(void*, const rapidjson::Value&, DeserializeContext)>;

  void RegisterComponentSerializeFunction(MyECS::CmptType, SerializeFunc);
  void RegisterComponentDeserializeFunction(MyECS::CmptType, DeserializeFunc);

  template <typename Func>
  void RegisterComponentSerializeFunction(Func&& func);
  template <typename Cmpt>
  void RegisterComponentSerializeFunction();

  template <typename Func>
  void RegisterComponentDeserializeFunction(Func&& func);
  template <typename Cmpt>
  void RegisterComponentDeserializeFunction();

  template <typename Func>
  void RegisterUserTypeSerializeFunction(Func&& func);
  template <typename Func>
  void RegisterUserTypeDeserializeFunction(Func&& func);

  std::string ToJSON(const MyECS::World*);
  void ToWorld(MyECS::World*, std::string_view json);

 private:
  void RegisterUserTypeSerializeFunction(size_t id, SerializeFunc func);
  void RegisterUserTypeDeserializeFunction(size_t id, DeserializeFunc func);

  Serializer();
  ~Serializer();

  struct Impl;
  Impl* pImpl;
};
}  // namespace My::MyGE

#include "details/Serializer.inl"
