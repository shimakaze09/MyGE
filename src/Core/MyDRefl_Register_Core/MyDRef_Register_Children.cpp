#include <MyGE/Core/Components/Children.h>

#include <UDRefl/UDRefl.hpp>

#include "UDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::UDRefl;

void Smkz::MyGE::details::UDRefl_Register_Children() {
  Mngr.RegisterType<Children>();
  Mngr.AddField<&Children::value>("value");
}
