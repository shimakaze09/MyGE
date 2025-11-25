#include <MyDRefl/MyDRefl.hpp>
#include <MyGM/MyGM.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::MyDRefl_Register_MyGM_1() {
  Mngr.RegisterType<pointf2>();
  Mngr.RegisterType<pointf3>();
  Mngr.RegisterType<pointf4>();
}

