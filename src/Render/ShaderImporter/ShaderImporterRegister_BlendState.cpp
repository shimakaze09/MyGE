#include <MyGE/Render/RenderState.h>

#include "ShaderImporterRegisterImpl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::ShaderImporterRegister_BlendState() {
  MyDRefl::Mngr.RegisterType<BlendState>();
  MyDRefl::Mngr.AddField<&BlendState::enable>("enable");
  MyDRefl::Mngr.AddField<&BlendState::src>("src");
  MyDRefl::Mngr.AddField<&BlendState::dest>("dest");
  MyDRefl::Mngr.AddField<&BlendState::op>("op");
  MyDRefl::Mngr.AddField<&BlendState::srcAlpha>("srcAlpha");
  MyDRefl::Mngr.AddField<&BlendState::destAlpha>("destAlpha");
  MyDRefl::Mngr.AddField<&BlendState::opAlpha>("opAlpha");
}
