#include <MyGE/Core/Components/LocalToWorld.h>
#include <MyGE/Core/Components/NonUniformScale.h>
#include <MyGE/Core/Components/Rotation.h>
#include <MyGE/Core/Components/Scale.h>
#include <MyGE/Core/Components/Translation.h>
#include <MyGE/Core/Systems/TRSToLocalToWorldSystem.h>

using namespace Smkz::MyGE;
using namespace Smkz::MyECS;

void TRSToLocalToWorldSystem::OnUpdate(Schedule& schedule) {
  schedule.RegisterChunkJob(
      [](ChunkView chunk) {
        auto chunkL2W = chunk->GetCmptArray<LocalToWorld>();
        auto chunkT = chunk->GetCmptArray<Translation>();
        auto chunkR = chunk->GetCmptArray<Rotation>();
        auto chunkS = chunk->GetCmptArray<Scale>();
        auto chunkNUS = chunk->GetCmptArray<NonUniformScale>();

        bool containsT = !chunkT.empty();
        bool containsR = !chunkR.empty();
        bool containsS = !chunkS.empty() || !chunkNUS.empty();
        assert(containsT || containsR || containsS);

        for (size_t i = 0; i < chunk->EntityNum(); i++) {
          scalef3 s = !chunkS.empty() ? chunkS[i].value : 1.f;
          if (!chunkNUS.empty()) s *= chunkNUS[i].value;

          // 00
          if (!containsT && !containsR) chunkL2W[i].value = transformf{s};
          // 01
          else if (!containsT && containsR)
            chunkL2W[i].value = transformf{chunkR[i].value, s};
          // 10
          else if (containsT && !containsR)
            chunkL2W[i].value = transformf{chunkT[i].value, s};
          // 11
          else  // if (containsT && containsR)
            chunkL2W[i].value = transformf{chunkT[i].value, chunkR[i].value, s};
        }
      },
      SystemFuncName,
      Schedule::ChunkJobConfig{
          .archetypeFilter = {.all = {AccessTypeID_of<Write<LocalToWorld>>},
                              .any = {
                                  AccessTypeID_of<Latest<Translation>>,
                                  AccessTypeID_of<Latest<Rotation>>,
                                  AccessTypeID_of<Latest<Scale>>,
                                  AccessTypeID_of<Latest<NonUniformScale>>,
                              }}});
}
