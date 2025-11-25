#include <MyGE/Core/Components/Roamer.h>

#include <MyDRefl/MyDRefl.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::MyDRefl_Register_Roamer() {
  Mngr.RegisterType<Roamer>();
  Mngr.AddField<&Roamer::moveSpeed>("moveSpeed");
  Mngr.AddField<&Roamer::rotateSpeed>("rotateSpeed");
  Mngr.AddField<&Roamer::reverseUpDown>("reverseUpDown");
  Mngr.AddField<&Roamer::reverseLeftRight>("reverseLeftRight");
  Mngr.AddField<&Roamer::reverseFrontBack>("reverseFrontBack");
}
