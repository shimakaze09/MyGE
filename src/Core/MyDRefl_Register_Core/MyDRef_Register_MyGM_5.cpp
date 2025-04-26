#include <MyDRefl/MyDRefl.hpp>
#include <MyGM/MyGM.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::MyDRefl_Register_MyGM_5() {
  Mngr.RegisterType<trianglef2>();
  Mngr.RegisterType<trianglef3>();
  Mngr.RegisterType<bboxf2>();
  Mngr.RegisterType<bboxf3>();
}
