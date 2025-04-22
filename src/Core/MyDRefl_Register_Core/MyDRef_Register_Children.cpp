#include <MyGE/Core/Components/Children.h>

#include <MyDRefl/MyDRefl.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::MyDRefl_Register_Children() {
  Mngr.RegisterType<Children>();
  Mngr.AddField<&Children::value>("value");
}
