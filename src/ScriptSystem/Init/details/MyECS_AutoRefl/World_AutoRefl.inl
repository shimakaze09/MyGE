// This file is generated by My::MySRefl::AutoRefl

#pragma once

#include <MySRefl/MySRefl.h>
#include <MyTemplate/Func.h>

template <>
struct My::MySRefl::TypeInfo<My::MyECS::World>
    : My::MySRefl::TypeInfoBase<My::MyECS::World> {
  static constexpr AttrList attrs = {};

  static constexpr FieldList fields = {
      Field{Name::constructor, WrapConstructor<My::MyECS::World()>()},
      Field{Name::constructor,
            WrapConstructor<My::MyECS::World(const My::MyECS::World&)>()},
      Field{"entityMngr", &My::MyECS::World::entityMngr},
      Field{"systemMngr", &My::MyECS::World::systemMngr},
      Field{"Update", &My::MyECS::World::Update},
      Field{"DumpUpdateJobGraph", &My::MyECS::World::DumpUpdateJobGraph},
      Field{"GenUpdateFrameGraph", &My::MyECS::World::GenUpdateFrameGraph},
      Field{"AddCommand", &My::MyECS::World::AddCommand},
      // Field{"Accept", &My::MyECS::World::Accept},
      Field{"RunEntityJob",
            My::DecayLambda(
                [](My::MyECS::World* world,
                   std::function<void(
                       My::MyECS::World*, My::MyECS::SingletonsView,
                       My::MyECS::Entity, size_t, My::MyECS::CmptsView)>
                       func,
                   My::MyECS::ArchetypeFilter archetypeFilter,
                   My::MyECS::CmptLocator cmptLocator,
                   My::MyECS::SingletonLocator singletonLocator,
                   bool isParallel) {
                  world->RunEntityJob(
                      std::move(func), isParallel, std::move(archetypeFilter),
                      std::move(cmptLocator), std::move(singletonLocator));
                })},
      Field{"RunChunkJob",
            My::DecayLambda(
                [](My::MyECS::World* world,
                   std::function<void(My::MyECS::World*, My::MyECS::ChunkView,
                                      My::MyECS::SingletonsView)>
                       func,
                   My::MyECS::ArchetypeFilter archetypeFilter,
                   My::MyECS::SingletonLocator singletonLocator,
                   bool isParallel) {
                  world->RunChunkJob(std::move(func),
                                     std::move(archetypeFilter), isParallel,
                                     std::move(singletonLocator));
                })},
  };
};
