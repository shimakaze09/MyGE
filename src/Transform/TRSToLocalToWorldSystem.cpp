//
// Created by Admin on 13/03/2025.
//

#include <MyGE/Transform/Components/LocalToWorld.h>
#include <MyGE/Transform/Components/Rotation.h>
#include <MyGE/Transform/Components/Scale.h>
#include <MyGE/Transform/Components/Translation.h>
#include <MyGE/Transform/Systems/TRSToLocalToWorldSystem.h>

using namespace My::MyGE;

void TRSToLocalToWorldSystem::OnUpdate(MyECS::Schedule& schedule) {
  MyECS::ArchetypeFilter filter{
      TypeList<LocalToWorld>{},
      TypeList<MyECS::Latest<Translation>, MyECS::Latest<Rotation>,
               MyECS::Latest<Scale>>{},
      TypeList<>{},
  };

  schedule.Register(
      [](MyECS::ChunkView chunk) {
        auto chunkL2W = chunk.GetCmptArray<LocalToWorld>();
        auto chunkT = chunk.GetCmptArray<Translation>();
        auto chunkR = chunk.GetCmptArray<Rotation>();
        auto chunkS = chunk.GetCmptArray<Scale>();

        bool containsT = chunkT != nullptr;
        bool containsR = chunkR != nullptr;
        bool containsS = chunkS != nullptr;

        for (size_t i = 0; i < chunk.EntityNum(); i++) {
          // 000
          if (!containsT && !containsR && !containsS) {
            assert(false);
          }
          // 001
          else if (!containsT && !containsR && containsS) {
            chunkL2W[i].value = transformf{chunkS[i].value};
          }
          // 010
          else if (!containsT && containsR && !containsS) {
            chunkL2W[i].value = transformf{chunkR[i].value};
          }
          // 011
          else if (!containsT && containsR && containsS) {
            chunkL2W[i].value = transformf{chunkR[i].value, chunkS[i].value};
          }
          // 100
          else if (containsT && !containsR && !containsS) {
            chunkL2W[i].value = transformf{chunkT[i].value};
          }
          // 101
          else if (containsT && !containsR && containsS) {
            chunkL2W[i].value =
                transformf{chunkT[i].value, scalef3{chunkS[i].value}};
          }
          // 110
          else if (containsT && containsR && !containsS) {
            chunkL2W[i].value = transformf{chunkT[i].value, chunkR[i].value};
          }
          // 111
          else /* if (containsT && containsR && containsS)*/ {
            chunkL2W[i].value = transformf{chunkT[i].value, chunkR[i].value,
                                           scalef3{chunkS[i].value}};
          }
        }
      },
      SystemFuncName, filter);
}
