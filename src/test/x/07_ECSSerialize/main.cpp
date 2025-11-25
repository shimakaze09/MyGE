#include <MyGE/Core/AssetMngr.h>
#include <MyGE/Core/Components/Components.h>
#include <MyGE/Core/Register_Core.h>
#include <MyGE/Core/Serializer.h>
#include <MyGE/Core/SharedVar.h>
#include <assert.h>

#include <MyECS/MyECS.hpp>
#include <MyGM/MyGM.hpp>
#include <_deps/crossguid/guid.hpp>
#include <iostream>

using namespace My;
using namespace My::MyGE;
using namespace My::MyECS;

int main() {
  // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  MyDRefl_Register_Core();

  {  // empty world
    World w;
    std::string str = Serializer::Instance().Serialize(&w);
    assert(str == R"({"__EntityMngr":{"__Entities":[]}})");
  }
  {  // world -> empty world
    World w;
    World_Register_Core(&w);

    {
      auto e =
          w.entityMngr.Create(TypeIDs_of<Translation, Scale, NonUniformScale,
                                         Rotation, RotationEuler>);
      w.entityMngr.WriteComponent<Translation>(e)->value = {3.f, 2.f, 1.f};
      w.entityMngr.WriteComponent<Scale>(e)->value = 3.5f;
      w.entityMngr.WriteComponent<NonUniformScale>(e)->value = {4.f, 5.f, 6.f};
    }
    w.entityMngr.Create(TypeIDs_of<Roamer>);
    w.entityMngr.Create(TypeIDs_of<Input>);
    {
      auto e = w.entityMngr.Create(TypeIDs_of<WorldTime, MyGE::Name>);
      w.entityMngr.WriteComponent<MyGE::Name>(e)->value = "hello";
    }
    std::string str = Serializer::Instance().Serialize(&w);
    std::cout << str << std::endl;

    World w2;
    World_Register_Core(&w2);
    auto success = Serializer::Instance().DeserializeToWorld(&w2, str);
    assert(success);
    assert(w.entityMngr.ReadSingleton<Translation>()->value ==
           w2.entityMngr.ReadSingleton<Translation>()->value);
    assert(w.entityMngr.ReadSingleton<Scale>()->value ==
           w2.entityMngr.ReadSingleton<Scale>()->value);
    assert(w.entityMngr.ReadSingleton<NonUniformScale>()->value ==
           w2.entityMngr.ReadSingleton<NonUniformScale>()->value);
    assert(w.entityMngr.ReadSingleton<MyGE::Name>()->value ==
           w2.entityMngr.ReadSingleton<MyGE::Name>()->value);
  }
  {  // entity reference
    World w;
    World_Register_Core(&w);
    {
      auto e0 = w.entityMngr.Create(TypeIDs_of<Children>);
      auto e1 = w.entityMngr.Create(TypeIDs_of<Parent>);
      auto e2 = w.entityMngr.Create(TypeIDs_of<Parent>);
      w.entityMngr.WriteComponent<Children>(e0)->value = {e1, e2};
      w.entityMngr.WriteComponent<Parent>(e1)->value = e0;
      w.entityMngr.WriteComponent<Parent>(e2)->value = e0;
    }

    std::string str = Serializer::Instance().Serialize(&w);
    std::cout << str << std::endl;

    World w2;
    World_Register_Core(&w2);
    {
      auto e0 = w2.entityMngr.Create(TypeIDs_of<Translation>);
      auto e1 = w2.entityMngr.Create(TypeIDs_of<Roamer>);
      w2.entityMngr.Destroy(e0);  // make a hole in entity table
    }
    auto success = Serializer::Instance().DeserializeToWorld(&w2, str);
    assert(success);
    auto p = w2.entityMngr.GetSingletonEntity(TypeID_of<Children>);
    auto children = w2.entityMngr.ReadComponent<Children>(p)->value;
    for (Entity e : children) {
      assert(w2.entityMngr.ReadComponent<Parent>(e)->value == p);
      assert(!w2.entityMngr.ReadComponent<Translation>(e));
    }
    std::string str2 = Serializer::Instance().Serialize(&w2);
    std::cout << str2 << std::endl;
  }
  {  // world entities
    World w;
    World_Register_Core(&w);

    Entity e[2];

    e[0] = w.entityMngr.Create(TypeIDs_of<Translation>);
    w.entityMngr.WriteComponent<Translation>(e[0])->value = {3.f, 2.f, 1.f};
    e[1] = w.entityMngr.Create(TypeIDs_of<Scale>);
    w.entityMngr.WriteComponent<Scale>(e[1])->value = {3.f};

    std::string str = Serializer::Instance().Serialize(&w, e);
    std::cout << str << std::endl;

    World w2;
    World_Register_Core(&w2);
    auto success = Serializer::Instance().DeserializeToWorld(&w2, str);
    assert(success);
    assert(w.entityMngr.ReadSingleton<Translation>()->value ==
           w2.entityMngr.ReadSingleton<Translation>()->value);
    assert(w.entityMngr.ReadSingleton<Scale>()->value ==
           w2.entityMngr.ReadSingleton<Scale>()->value);
  }
}