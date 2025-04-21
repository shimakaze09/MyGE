#include <MyGE/Core/Components/WorldTime.h>
#include <MyGE/Core/GameTimer.h>
#include <MyGE/Core/Systems/WorldTimeSystem.h>

using namespace Smkz::MyGE;
using namespace Smkz::MyECS;

void WorldTimeSystem::OnUpdate(Schedule& schedule) {
  schedule.RegisterJob(
      [](Singleton<WorldTime> time) {
        time->elapsedTime = GameTimer::Instance().TotalTime();
        time->deltaTime = GameTimer::Instance().DeltaTime();
      },
      SystemFuncName);
}
