//
// Created by Admin on 17/03/2025.
//

#pragma once

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
  using CmptSerializeFunc = std::function<void(const void*, JSONWriter&)>;
  using JSONCmpt = rapidjson::GenericObject<true, rapidjson::Value>;
  using CmptDeserializeFunc =
      std::function<void(MyECS::World*, MyECS::Entity, const JSONCmpt&)>;

  void RegisterComponentSerializeFunction(MyECS::CmptType, CmptSerializeFunc);
  void RegisterComponentDeserializeFunction(MyECS::CmptType,
                                            CmptDeserializeFunc);
  template <typename Func>
  void RegisterComponentSerializeFunction(Func&& func);
  template <typename Cmpt>
  void RegisterComponentSerializeFunction();
  template <typename Cmpt>
  void RegisterComponentDeserializeFunction();

  std::string ToJSON(const MyECS::World*);
  MyECS::World* ToWorld(std::string_view json);

 private:
  Serializer();
  ~Serializer();

  struct Impl;
  Impl* pImpl;
};
}  // namespace My::MyGE

#include "details/Serializer.inl"
