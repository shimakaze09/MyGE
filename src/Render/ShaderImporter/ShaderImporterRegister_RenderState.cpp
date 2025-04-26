#include <MyGE/Render/RenderState.h>

#include "ShaderImporterRegisterImpl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::ShaderImporterRegister_RenderState() {
  MyDRefl::Mngr.RegisterType<RenderState>();
  MyDRefl::Mngr.SimpleAddField<&RenderState::fillMode>("fillMode");
  MyDRefl::Mngr.SimpleAddField<&RenderState::cullMode>("cullMode");
  MyDRefl::Mngr.SimpleAddField<&RenderState::zTest>("zTest");
  MyDRefl::Mngr.SimpleAddField<&RenderState::zWrite>("zWrite");
  MyDRefl::Mngr.SimpleAddField<&RenderState::stencilState>("stencilState");
  MyDRefl::Mngr.AddField<&RenderState::blendStates>("blendStates");
  MyDRefl::Mngr.AddField<&RenderState::colorMask>("colorMask");
}
