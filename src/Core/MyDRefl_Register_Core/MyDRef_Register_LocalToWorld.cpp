#include <MyGE/Core/Components/LocalToWorld.h>

#include <UDRefl/UDRefl.hpp>

#include "UDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::UDRefl;

void Smkz::MyGE::details::UDRefl_Register_LocalToWorld() {
  Mngr.RegisterType<LocalToWorld>();
  Mngr.AddField<&LocalToWorld::value>("value");
}
