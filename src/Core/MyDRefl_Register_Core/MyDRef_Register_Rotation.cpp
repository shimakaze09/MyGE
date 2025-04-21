#include <MyGE/Core/Components/Rotation.h>

#include <UDRefl/UDRefl.hpp>

#include "UDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::UDRefl;

void Smkz::MyGE::details::UDRefl_Register_Rotation() {
  Mngr.RegisterType<Rotation>();
  Mngr.AddField<&Rotation::value>("value");
}
