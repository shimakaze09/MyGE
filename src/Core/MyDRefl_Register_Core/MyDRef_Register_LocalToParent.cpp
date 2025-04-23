#include <MyGE/Core/Components/LocalToParent.h>

#include <MyDRefl/MyDRefl.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::MyDRefl_Register_LocalToParent() {
  Mngr.RegisterType<LocalToParent>();
  Mngr.SimpleAddField<&LocalToParent::value>("value");
}
