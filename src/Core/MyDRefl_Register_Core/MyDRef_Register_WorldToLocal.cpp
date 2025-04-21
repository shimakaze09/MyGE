#include <MyGE/Core/Components/WorldToLocal.h>

#include <UDRefl/UDRefl.hpp>

#include "UDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::UDRefl;

void Smkz::MyGE::details::UDRefl_Register_WorldToLocal() {
  Mngr.RegisterType<WorldToLocal>();
  Mngr.AddField<&WorldToLocal::value>("value");
}
