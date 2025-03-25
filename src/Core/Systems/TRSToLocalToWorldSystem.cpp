#include <MyGE/Core/Systems/TRSToLocalToWorldSystem.h>

#include <MyGE/Core/Components/LocalToWorld.h>
#include <MyGE/Core/Components/NonUniformScale.h>
#include <MyGE/Core/Components/Rotation.h>
#include <MyGE/Core/Components/Scale.h>
#include <MyGE/Core/Components/Translation.h>

using namespace My::MyGE;

void TRSToLocalToWorldSystem::OnUpdate(MyECS::Schedule& schedule) {
  MyECS::ArchetypeFilter filter;
  filter.all = {MyECS::CmptAccessType::Of<MyECS::Write<LocalToWorld>>};
  filter.any = {
      MyECS::CmptAccessType::Of<MyECS::Latest<Translation>>,
      MyECS::CmptAccessType::Of<MyECS::Latest<Rotation>>,
      MyECS::CmptAccessType::Of<MyECS::Latest<Scale>>,
      MyECS::CmptAccessType::Of<MyECS::Latest<NonUniformScale>>,
  };

  schedule.RegisterChunkJob(
      [](MyECS::ChunkView chunk) {
        auto chunkL2W = chunk.GetCmptArray<LocalToWorld>();
        auto chunkT = chunk.GetCmptArray<Translation>();
        auto chunkR = chunk.GetCmptArray<Rotation>();
        auto chunkS = chunk.GetCmptArray<Scale>();
        auto chunkNUS = chunk.GetCmptArray<NonUniformScale>();

        bool containsT = chunkT != nullptr;
        bool containsR = chunkR != nullptr;
        bool containsS = chunkS != nullptr || chunkNUS != nullptr;
        assert(containsT || containsR && containsS);

        for (size_t i = 0; i < chunk.EntityNum(); i++) {
          scalef3 s = chunkS ? chunkS[i].value : 1.f;
          if (chunkNUS)
            s *= chunkNUS[i].value;

          // 00
          if (!containsT && !containsR) {
            chunkL2W[i].value = transformf{s};
          }
          // 01
          else if (!containsT && containsR) {
            chunkL2W[i].value = transformf{chunkR[i].value, s};
          }
          // 10
          else if (containsT && !containsR) {
            chunkL2W[i].value = transformf{chunkT[i].value, s};
          }
          // 11
          else /*if (containsT && containsR)*/ {
            chunkL2W[i].value = transformf{chunkT[i].value, chunkR[i].value, s};
          }
        }
      },
      SystemFuncName, filter);
}
