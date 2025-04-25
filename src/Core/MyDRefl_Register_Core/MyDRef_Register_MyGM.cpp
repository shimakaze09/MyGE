#include <MyDRefl/MyDRefl.hpp>
#include <MyGM/MyGM.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::MyDRefl_Register_MyGM() {
  Mngr.RegisterType<vecf2>();
  Mngr.RegisterType<vecf3>();
  Mngr.RegisterType<vecf4>();
  Mngr.RegisterType<pointf2>();
  Mngr.RegisterType<pointf3>();
  Mngr.RegisterType<pointf4>();
  Mngr.RegisterType<valf2>();
  Mngr.RegisterType<valf3>();
  Mngr.RegisterType<valf4>();
  Mngr.RegisterType<matf2>();
  Mngr.RegisterType<matf3>();
  Mngr.RegisterType<matf4>();
  Mngr.RegisterType<transformf>();
  Mngr.RegisterType<eulerf>();
  Mngr.RegisterType<quatf>();
  Mngr.RegisterType<rgbf>();
  Mngr.RegisterType<rgbaf>();
  Mngr.RegisterType<trianglef2>();
  Mngr.RegisterType<trianglef3>();
  Mngr.RegisterType<bboxf2>();
  Mngr.RegisterType<bboxf3>();
}