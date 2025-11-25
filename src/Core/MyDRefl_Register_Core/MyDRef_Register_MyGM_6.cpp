#include <MyDRefl/MyDRefl.hpp>
#include <MyGM/MyGM.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::MyDRefl_Register_MyGM_6() {
  Mngr.RegisterType<scalef2>();
  Mngr.RegisterType<scalef3>();
  Mngr.RegisterType<scalef4>();
}

