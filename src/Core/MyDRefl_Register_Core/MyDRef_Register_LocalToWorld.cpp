#include <MyGE/Core/Components/LocalToWorld.h>

#include <MyDRefl/MyDRefl.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::MyDRefl_Register_LocalToWorld() {
  Mngr.RegisterType<LocalToWorld>();
  Mngr.SimpleAddField<&LocalToWorld::value>("value");
}
