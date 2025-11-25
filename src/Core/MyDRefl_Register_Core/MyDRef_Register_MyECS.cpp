#include <MyDRefl/MyDRefl.hpp>
#include <MyECS/Entity.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::MyDRefl_Register_MyECS() {
  Mngr.RegisterType<MyECS::Entity>();
  Mngr.AddField<&MyECS::Entity::index>("index");
  Mngr.AddField<&MyECS::Entity::version>("version");
}
