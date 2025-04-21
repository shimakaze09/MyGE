#include <MyGE/Core/Components/LocalToParent.h>

#include <UDRefl/UDRefl.hpp>

#include "UDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::UDRefl;

void Smkz::MyGE::details::UDRefl_Register_LocalToParent() {
  Mngr.RegisterType<LocalToParent>();
  Mngr.AddField<&LocalToParent::value>("value");
}
