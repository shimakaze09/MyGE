#include <MyDRefl/MyDRefl.hpp>
#include <MyGM/MyGM.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::MyDRefl_Register_MyGM_2() {
  Mngr.RegisterType<valf2>();
  Mngr.RegisterType<valf3>();
  Mngr.RegisterType<valf4>();
}

