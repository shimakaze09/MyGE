#include <MyGE/Core/Components/LocalToParent.h>

#include <MyDRefl/MyDRefl.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::MyDRefl_Register_LocalToParent() {
  Mngr.RegisterType<LocalToParent>();
  Mngr.AddField<&LocalToParent::value>("value");
}

