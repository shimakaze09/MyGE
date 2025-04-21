#include <MyGE/Core/Components/Scale.h>

#include <UDRefl/UDRefl.hpp>

#include "UDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::UDRefl;

void Smkz::MyGE::details::UDRefl_Register_Scale() {
  Mngr.RegisterType<Scale>();
  Mngr.AddField<&Scale::value>("value");
}
