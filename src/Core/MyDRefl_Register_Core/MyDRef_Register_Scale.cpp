#include <MyGE/Core/Components/Scale.h>

#include <MyDRefl/MyDRefl.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::MyDRefl_Register_Scale() {
  Mngr.RegisterType<Scale>();
  Mngr.AddField<&Scale::value>("value");
}
