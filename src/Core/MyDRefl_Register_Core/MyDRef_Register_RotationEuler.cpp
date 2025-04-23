#include <MyGE/Core/Components/RotationEuler.h>

#include <MyDRefl/MyDRefl.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::MyDRefl_Register_RotationEuler() {
  Mngr.RegisterType<RotationEuler>();
  Mngr.SimpleAddField<&RotationEuler::value>("value");
}
