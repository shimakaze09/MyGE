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

  struct Key {
    static constexpr const char ENTITY_MNGR[] = "__ENTITY_MNGR";
    static constexpr const char ENTITIES[] = "__ENTITIES";
    static constexpr const char INDEX[] = "__INDEX";
    static constexpr const char COMPONENTS[] = "__COMPONENTS";
    static constexpr const char TYPE[] = "__TYPE";
    static constexpr const char CONTENT[] = "__CONTENT";
    static constexpr const char NOT_SUPPORT[] = "__NOT_SUPPORT";
    static constexpr const char KEY[] = "__KEY";
    static constexpr const char MAPPED[] = "__MAPPED";
  };

  using JSONWriter = rapidjson::Writer<rapidjson::StringBuffer>;
  struct SerializeContext {
    JSONWriter* const writer;
    const Visitor<void(const void*, SerializeContext)>* const serializer;
  };
  using SerializeFunc = std::function<void(const void*, SerializeContext)>;
  using EntityIdxMap = std::unordered_map<size_t, MyECS::Entity>;
  struct DeserializeContext {
    const EntityIdxMap* entityIdxMap;
    const Visitor<void(void*, const rapidjson::Value&,
                       DeserializeContext)>* const deserializer;
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

  void RegisterUserTypeSerializeFunction(size_t id, SerializeFunc);
  void RegisterUserTypeDeserializeFunction(size_t id, DeserializeFunc);
  template <typename Func>
  void RegisterUserTypeSerializeFunction(Func&& func);
  template <typename Func>
  void RegisterUserTypeDeserializeFunction(Func&& func);

  std::string ToJSON(const MyECS::World*);
  void ToWorld(MyECS::World*, std::string_view json);

 private:
  // core functions
  void RegisterSerializeFunction(size_t id, SerializeFunc);
  void RegisterDeserializeFunction(size_t id, DeserializeFunc);

  Serializer();
  ~Serializer();

  struct Impl;
  Impl* pImpl;
};
}  // namespace My::MyGE

#include "details/Serializer.inl"
