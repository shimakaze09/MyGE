#include <MyGE/Core/Components/Scale.h>

#include <MyDRefl/MyDRefl.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::MyDRefl_Register_Scale() {
  Mngr.RegisterType<Scale>();
  Mngr.AddField<&Scale::value>("value");
}

