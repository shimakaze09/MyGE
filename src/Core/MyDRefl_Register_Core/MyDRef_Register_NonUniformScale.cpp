#include <MyGE/Core/Components/NonUniformScale.h>

#include <MyDRefl/MyDRefl.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::MyDRefl_Register_NonUniformScale() {
  Mngr.RegisterType<NonUniformScale>();
  Mngr.AddField<&NonUniformScale::value>("value");
}
