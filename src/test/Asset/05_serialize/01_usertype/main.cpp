//
// Created by Admin on 21/03/2025.
//

#include <MyGE/Asset/Serializer.h>

#include <iostream>

using namespace My::MyGE;
using namespace std;

struct Type {
  std::vector<std::map<std::string, std::string>> data;
};

template <>
struct My::MySRefl::TypeInfo<Type> : My::MySRefl::TypeInfoBase<Type> {
  static constexpr AttrList attrs = {};

  static constexpr FieldList fields = {
      Field{"data", &Type::data},
  };
};

int main() {
  // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  Serializer::Instance().RegisterUserTypes<Type>();

  Type t;
  t.data = {{{"a", "b"}, {"c", "d"}}, {{"e", "f"}, {"g", "h"}}};

  auto json = Serializer::Instance().ToJSON(&t);
  std::cout << json << std::endl;

  t.data.clear();
  Serializer::Instance().ToUserType(json, &t);

  auto newjson = Serializer::Instance().ToJSON(&t);
  std::cout << newjson << std::endl;

  return 0;
}
