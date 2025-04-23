#include <MyGE/Core/Components/Roamer.h>

#include <MyDRefl/MyDRefl.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::MyDRefl_Register_Roamer() {
  Mngr.RegisterType<Roamer>();
  Mngr.SimpleAddField<&Roamer::moveSpeed>("moveSpeed");
  Mngr.SimpleAddField<&Roamer::rotateSpeed>("rotateSpeed");
  Mngr.SimpleAddField<&Roamer::reverseUpDown>("reverseUpDown");
  Mngr.SimpleAddField<&Roamer::reverseLeftRight>("reverseLeftRight");
  Mngr.SimpleAddField<&Roamer::reverseFrontBack>("reverseFrontBack");
}
