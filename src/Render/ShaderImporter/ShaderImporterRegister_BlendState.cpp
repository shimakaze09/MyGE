#include <MyGE/Render/RenderState.h>

#include "ShaderImporterRegisterImpl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::ShaderImporterRegister_BlendState() {
  MyDRefl::Mngr.RegisterType<BlendState>();
  MyDRefl::Mngr.SimpleAddField<&BlendState::enable>("enable");
  MyDRefl::Mngr.SimpleAddField<&BlendState::src>("src");
  MyDRefl::Mngr.SimpleAddField<&BlendState::dest>("dest");
  MyDRefl::Mngr.SimpleAddField<&BlendState::op>("op");
  MyDRefl::Mngr.SimpleAddField<&BlendState::srcAlpha>("srcAlpha");
  MyDRefl::Mngr.SimpleAddField<&BlendState::destAlpha>("destAlpha");
  MyDRefl::Mngr.SimpleAddField<&BlendState::opAlpha>("opAlpha");
}
