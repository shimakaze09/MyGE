#include <MyDRefl/MyDRefl.hpp>
#include <MyGM/MyGM.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::MyDRefl_Register_MyGM_5() {
  Mngr.RegisterType<trianglef2>();
  Mngr.RegisterType<trianglef3>();
  Mngr.RegisterType<bboxf2>();
  Mngr.RegisterType<bboxf3>();
}

