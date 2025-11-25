#include <MyGE/Render/RenderState.h>

#include "ShaderImporterRegisterImpl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::ShaderImporterRegister_CullMode() {
  MyDRefl::Mngr.RegisterType<CullMode>();
  MyDRefl::Mngr.AddField<CullMode::NONE>("NONE");
  MyDRefl::Mngr.AddField<CullMode::FRONT>("FRONT");
  MyDRefl::Mngr.AddField<CullMode::BACK>("BACK");
}
