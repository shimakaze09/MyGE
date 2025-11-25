#include <MyGE/Render/RenderState.h>

#include "ShaderImporterRegisterImpl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::ShaderImporterRegister_RenderState() {
  MyDRefl::Mngr.RegisterType<RenderState>();
  MyDRefl::Mngr.AddField<&RenderState::fillMode>("fillMode");
  MyDRefl::Mngr.AddField<&RenderState::cullMode>("cullMode");
  MyDRefl::Mngr.AddField<&RenderState::zTest>("zTest");
  MyDRefl::Mngr.AddField<&RenderState::zWrite>("zWrite");
  MyDRefl::Mngr.AddField<&RenderState::stencilState>("stencilState");
  MyDRefl::Mngr.AddField<&RenderState::blendStates>("blendStates");
  MyDRefl::Mngr.AddField<&RenderState::colorMask>("colorMask");
}
