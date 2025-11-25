#include <MyGE/Core/Components/Rotation.h>

#include <MyDRefl/MyDRefl.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::MyDRefl_Register_Rotation() {
  Mngr.RegisterType<Rotation>();
  Mngr.AddField<&Rotation::value>("value");
}
