#include <MyGE/Core/Components/Translation.h>

#include <MyDRefl/MyDRefl.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::MyDRefl_Register_Translation() {
  Mngr.RegisterType<Translation>();
  Mngr.SimpleAddField<&Translation::value>("value");
}
