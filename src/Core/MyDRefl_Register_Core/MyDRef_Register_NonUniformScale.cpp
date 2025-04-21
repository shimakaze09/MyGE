#include <MyGE/Core/Components/NonUniformScale.h>

#include <UDRefl/UDRefl.hpp>

#include "UDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::UDRefl;

void Smkz::MyGE::details::UDRefl_Register_NonUniformScale() {
  Mngr.RegisterType<NonUniformScale>();
  Mngr.AddField<&NonUniformScale::value>("value");
}
