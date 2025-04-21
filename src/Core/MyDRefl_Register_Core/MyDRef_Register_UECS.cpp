#include <MyECS/Entity.hpp>
#include <UDRefl/UDRefl.hpp>

#include "UDRefl_Register_Core_impl.h"

using namespace Smkz::MyGE;
using namespace Smkz::UDRefl;

void Smkz::MyGE::details::UDRefl_Register_MyECS() {
  Mngr.RegisterType<MyECS::Entity>();
  Mngr.AddField<&MyECS::Entity::index>("index");
  Mngr.AddField<&MyECS::Entity::version>("version");
}
