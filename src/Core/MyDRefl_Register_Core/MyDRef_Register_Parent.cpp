#include <MyGE/Core/Components/Parent.h>

#include <MyDRefl/MyDRefl.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::MyDRefl_Register_Parent() {
  Mngr.RegisterType<Parent>();
  Mngr.AddField<&Parent::value>("value");
}
