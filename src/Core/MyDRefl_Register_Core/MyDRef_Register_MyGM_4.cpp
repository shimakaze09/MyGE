#include <MyDRefl/MyDRefl.hpp>
#include <MyGM/MyGM.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::MyDRefl_Register_MyGM_4() {
  Mngr.RegisterType<transformf>();
  Mngr.RegisterType<eulerf>();
  Mngr.RegisterType<quatf>();
  Mngr.RegisterType<rgbf>();
  Mngr.RegisterType<rgbaf>();
}
