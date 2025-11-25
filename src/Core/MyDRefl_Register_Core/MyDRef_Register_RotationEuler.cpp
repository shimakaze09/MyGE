#include <MyGE/Core/Components/RotationEuler.h>

#include <MyDRefl/MyDRefl.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::MyDRefl_Register_RotationEuler() {
  Mngr.RegisterType<RotationEuler>();
  Mngr.AddField<&RotationEuler::value>("value");
}

