#include <MyGE/Core/Systems/TRSToLocalToParentSystem.h>

#include <MyGE/Core/Components/LocalToParent.h>
#include <MyGE/Core/Components/NonUniformScale.h>
#include <MyGE/Core/Components/Parent.h>
#include <MyGE/Core/Components/Rotation.h>
#include <MyGE/Core/Components/Scale.h>
#include <MyGE/Core/Components/Translation.h>

using namespace My::MyGE;

void TRSToLocalToParentSystem::OnUpdate(MyECS::Schedule& schedule) {
  MyECS::ArchetypeFilter filter;
  filter.all = {MyECS::CmptAccessType::Of<MyECS::Write<LocalToParent>>,
                MyECS::CmptAccessType::Of<MyECS::Latest<Parent>>};
  filter.any = {
      MyECS::CmptAccessType::Of<MyECS::Latest<Translation>>,
      MyECS::CmptAccessType::Of<MyECS::Latest<Rotation>>,
      MyECS::CmptAccessType::Of<MyECS::Latest<Scale>>,
      MyECS::CmptAccessType::Of<MyECS::Latest<NonUniformScale>>,
  };

  schedule.RegisterChunkJob(
      [](MyECS::ChunkView chunk) {
        auto chunkL2P = chunk.GetCmptArray<LocalToParent>();
        auto chunkT = chunk.GetCmptArray<Translation>();
        auto chunkR = chunk.GetCmptArray<Rotation>();
        auto chunkS = chunk.GetCmptArray<Scale>();
        auto chunkNUS = chunk.GetCmptArray<NonUniformScale>();

        bool containsT = !chunkT.empty();
        bool containsR = !chunkR.empty();
        bool containsS = !chunkS.empty() || !chunkNUS.empty();
        assert(containsT || containsR || containsS);

        for (size_t i = 0; i < chunk.EntityNum(); i++) {
          scalef3 s = !chunkS.empty() ? chunkS[i].value : 1.f;
          if (!chunkNUS.empty())
            s *= chunkNUS[i].value;

          // 00
          if (!containsT && !containsR)
            chunkL2P[i].value = transformf{s};
          // 01
          else if (!containsT && containsR)
            chunkL2P[i].value = transformf{chunkR[i].value, s};
          // 10
          else if (containsT && !containsR)
            chunkL2P[i].value = transformf{chunkT[i].value, s};
          // 11
          else  // if (containsT && containsR)
            chunkL2P[i].value = transformf{chunkT[i].value, chunkR[i].value, s};
        }
      },
      SystemFuncName, filter);
}
