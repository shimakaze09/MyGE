#include <MyGE/Core/Components/Name.h>

#include <MyDRefl/MyDRefl.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::MyDRefl_Register_Name() {
  Mngr.RegisterType<Name>();
  Mngr.AddField<&Name::value>("value");
}
