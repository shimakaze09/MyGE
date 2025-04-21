#include <MyGE/Core/Components/Translation.h>

#include <UDRefl/UDRefl.hpp>

#include "UDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::UDRefl;

void Smkz::MyGE::details::UDRefl_Register_Translation() {
  Mngr.RegisterType<Translation>();
  Mngr.AddField<&Translation::value>("value");
}
