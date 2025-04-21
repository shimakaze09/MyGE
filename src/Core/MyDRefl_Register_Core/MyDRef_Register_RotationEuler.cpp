#include <MyGE/Core/Components/RotationEuler.h>

#include <UDRefl/UDRefl.hpp>

#include "UDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::UDRefl;

void Smkz::MyGE::details::UDRefl_Register_RotationEuler() {
  Mngr.RegisterType<RotationEuler>();
  Mngr.AddField<&RotationEuler::value>("value");
}
