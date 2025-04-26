#include <MyGE/Render/RenderState.h>

#include "ShaderImporterRegisterImpl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::ShaderImporterRegister_CullMode() {
  MyDRefl::Mngr.RegisterType<CullMode>();
  MyDRefl::Mngr.SimpleAddField<CullMode::NONE>("NONE");
  MyDRefl::Mngr.SimpleAddField<CullMode::FRONT>("FRONT");
  MyDRefl::Mngr.SimpleAddField<CullMode::BACK>("BACK");
}
