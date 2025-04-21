#include <MyGE/Core/Components/Roamer.h>

#include <UDRefl/UDRefl.hpp>

#include "UDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::UDRefl;

void Smkz::MyGE::details::UDRefl_Register_Roamer() {
  Mngr.RegisterType<Roamer>();
  Mngr.AddField<&Roamer::moveSpeed>("moveSpeed");
  Mngr.AddField<&Roamer::rotateSpeed>("rotateSpeed");
  Mngr.AddField<&Roamer::reverseUpDown>("reverseUpDown");
  Mngr.AddField<&Roamer::reverseLeftRight>("reverseLeftRight");
  Mngr.AddField<&Roamer::reverseFrontBack>("reverseFrontBack");
}
