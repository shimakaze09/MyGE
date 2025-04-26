#include <MyDRefl/MyDRefl.hpp>
#include <MyGM/MyGM.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::MyDRefl_Register_MyGM_2() {
  Mngr.RegisterType<valf2>();
  Mngr.RegisterType<valf3>();
  Mngr.RegisterType<valf4>();
}
