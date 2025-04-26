#include <MyDRefl/MyDRefl.hpp>
#include <MyGM/MyGM.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::MyDRefl_Register_MyGM_1() {
  Mngr.RegisterType<pointf2>();
  Mngr.RegisterType<pointf3>();
  Mngr.RegisterType<pointf4>();
}
