#include <MyGE/Core/Components/Name.h>

#include <MyDRefl/MyDRefl.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::MyDRefl_Register_Name() {
  Mngr.RegisterType<Name>();
  Mngr.AddField<&Name::value>("value");
}

