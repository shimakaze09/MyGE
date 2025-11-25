#include <MyGE/Core/Components/NonUniformScale.h>

#include <MyDRefl/MyDRefl.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::MyDRefl_Register_NonUniformScale() {
  Mngr.RegisterType<NonUniformScale>();
  Mngr.AddField<&NonUniformScale::value>("value");
}

