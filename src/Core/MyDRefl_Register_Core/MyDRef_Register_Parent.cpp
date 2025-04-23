#include <MyGE/Core/Components/Parent.h>

#include <MyDRefl/MyDRefl.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::MyDRefl_Register_Parent() {
  Mngr.RegisterType<Parent>();
  Mngr.SimpleAddField<&Parent::value>("value");
}
