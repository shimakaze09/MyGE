#include <MyGE/Core/Components/WorldToLocal.h>

#include <MyDRefl/MyDRefl.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::MyDRefl_Register_WorldToLocal() {
  Mngr.RegisterType<WorldToLocal>();
  Mngr.SimpleAddField<&WorldToLocal::value>("value");
}
