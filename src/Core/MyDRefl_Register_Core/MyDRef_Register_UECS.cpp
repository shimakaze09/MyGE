#include <MyDRefl/MyDRefl.hpp>
#include <MyECS/Entity.hpp>

#include "MyDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::MyDRefl_Register_MyECS() {
  Mngr.RegisterType<MyECS::Entity>();
  Mngr.AddField<&MyECS::Entity::index>("index");
  Mngr.AddField<&MyECS::Entity::version>("version");
}
