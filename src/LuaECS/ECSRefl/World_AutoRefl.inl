// This file is generated by My::MySRefl::AutoRefl

#pragma once

#include <MySRefl/MySRefl.h>

template <>
struct My::MySRefl::TypeInfo<My::MyECS::World>
    : My::MySRefl::TypeInfoBase<My::MyECS::World> {
  static constexpr AttrList attrs = {};

  static constexpr FieldList fields = {
      Field{Name::constructor, WrapConstructor<My::MyECS::World()>()},
      Field{"GetSystemMngr",
            static_cast<My::MyECS::SystemMngr* (*)(My::MyECS::World&)>(
                [](My::MyECS::World& w) { return &w.systemMngr; })},
      Field{"GetSystemMngr",
            static_cast<
                const My::MyECS::SystemMngr* (*)(const My::MyECS::World&)>(
                [](const My::MyECS::World& w) { return &w.systemMngr; })},
      Field{Name::constructor, WrapConstructor<My::MyECS::World()>()},
      Field{"GetEntityMngr",
            static_cast<My::MyECS::EntityMngr* (*)(My::MyECS::World&)>(
                [](My::MyECS::World& w) { return &w.entityMngr; })},
      Field{"GetEntityMngr",
            static_cast<
                const My::MyECS::EntityMngr* (*)(const My::MyECS::World&)>(
                [](const My::MyECS::World& w) { return &w.entityMngr; })},
      Field{"Update", &My::MyECS::World::Update},
      Field{"DumpUpdateJobGraph", &My::MyECS::World::DumpUpdateJobGraph},
      Field{"GenUpdateFrameGraph", &My::MyECS::World::GenUpdateFrameGraph} /*,
       Field{"Accept", &My::MyECS::World::Accept,
           AttrList {
               Attr{MY_MYSRefl_NAME_ARG(0),
                   AttrList{
                       Attr{Name::name, "listener"},
                   }
               },
           }
       },*/
  };
};
