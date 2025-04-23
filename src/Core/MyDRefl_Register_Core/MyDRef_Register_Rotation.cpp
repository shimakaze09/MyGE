#include <MyGE/Core/Components/Rotation.h>

#include <MyDRefl/MyDRefl.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::MyDRefl_Register_Rotation() {
  Mngr.RegisterType<Rotation>();
  Mngr.SimpleAddField<&Rotation::value>("value");
}
