#include <MyGE/Core/Components/Parent.h>

#include <UDRefl/UDRefl.hpp>

#include "UDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::UDRefl;

void Smkz::MyGE::details::UDRefl_Register_Parent() {
  Mngr.RegisterType<Parent>();
  Mngr.AddField<&Parent::value>("value");
}
