#include <MyGE/Core/Components/WorldTime.h>

#include <MyDRefl/MyDRefl.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::MyDRefl_Register_WorldTime() {
  Mngr.RegisterType<WorldTime>();
  Mngr.AddField<&WorldTime::deltaTime>("deltaTime");
  Mngr.AddField<&WorldTime::elapsedTime>("elapsedTime");
}

