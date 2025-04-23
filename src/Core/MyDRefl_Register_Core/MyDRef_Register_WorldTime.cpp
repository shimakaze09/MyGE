#include <MyGE/Core/Components/WorldTime.h>

#include <MyDRefl/MyDRefl.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::MyDRefl_Register_WorldTime() {
  Mngr.RegisterType<WorldTime>();
  Mngr.SimpleAddField<&WorldTime::deltaTime>("deltaTime");
  Mngr.SimpleAddField<&WorldTime::elapsedTime>("elapsedTime");
}
