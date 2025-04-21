#include <MyGE/Core/Components/Name.h>

#include <UDRefl/UDRefl.hpp>

#include "UDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::UDRefl;

void Smkz::MyGE::details::UDRefl_Register_Name() {
  Mngr.RegisterType<Name>();
  Mngr.AddField<&Name::value>("value");
}
