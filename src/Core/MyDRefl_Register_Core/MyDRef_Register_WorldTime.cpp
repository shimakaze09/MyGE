#include <MyGE/Core/Components/WorldTime.h>

#include <UDRefl/UDRefl.hpp>

#include "UDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::UDRefl;

void Smkz::MyGE::details::UDRefl_Register_WorldTime() {
  Mngr.RegisterType<WorldTime>();
  Mngr.AddField<&WorldTime::deltaTime>("deltaTime");
  Mngr.AddField<&WorldTime::elapsedTime>("elapsedTime");
}
