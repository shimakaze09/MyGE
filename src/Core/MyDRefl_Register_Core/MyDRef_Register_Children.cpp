#include <MyGE/Core/Components/Children.h>

#include <MyDRefl/MyDRefl.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::MyDRefl_Register_Children() {
  Mngr.RegisterType<Children>();
  Mngr.AddField<&Children::value>("value");
}

