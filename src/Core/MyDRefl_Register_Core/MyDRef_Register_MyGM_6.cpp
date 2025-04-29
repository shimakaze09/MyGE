#include <MyDRefl/MyDRefl.hpp>
#include <MyGM/MyGM.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::MyDRefl_Register_MyGM_6() {
  Mngr.RegisterType<scalef2>();
  Mngr.RegisterType<scalef3>();
  Mngr.RegisterType<scalef4>();
}
