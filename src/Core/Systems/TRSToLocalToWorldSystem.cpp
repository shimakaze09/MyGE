#include <MyGE/Core/Components/LocalToWorld.h>
#include <MyGE/Core/Components/Rotation.h>
#include <MyGE/Core/Components/Scale.h>
#include <MyGE/Core/Components/Translation.h>
#include <MyGE/Core/Systems/TRSToLocalToWorldSystem.h>

using namespace My::MyGE;

void TRSToLocalToWorldSystem::OnUpdate(MyECS::Schedule& schedule) {
  MyECS::ArchetypeFilter filter;
  filter.all = {MyECS::CmptAccessType::Of<MyECS::Write<LocalToWorld>>};
  filter.any = {
      MyECS::CmptAccessType::Of<MyECS::Latest<Translation>>,
      MyECS::CmptAccessType::Of<MyECS::Latest<Rotation>>,
      MyECS::CmptAccessType::Of<MyECS::Latest<Scale>>,
  };

  schedule.RegisterChunkJob(
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
