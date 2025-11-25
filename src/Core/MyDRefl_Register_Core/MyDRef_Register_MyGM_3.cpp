#include <MyDRefl/MyDRefl.hpp>
#include <MyGM/MyGM.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::MyDRefl_Register_MyGM_3() {
  Mngr.RegisterType<matf2>();
  Mngr.RegisterType<matf3>();
  Mngr.RegisterType<matf4>();
}

