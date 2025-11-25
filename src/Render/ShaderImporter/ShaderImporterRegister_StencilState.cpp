#include <MyGE/Render/RenderState.h>

#include "ShaderImporterRegisterImpl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::ShaderImporterRegister_StencilState() {
  MyDRefl::Mngr.RegisterType<StencilState>();
  MyDRefl::Mngr.AddField<&StencilState::enable>("enable");
  MyDRefl::Mngr.AddField<&StencilState::ref>("ref");
  MyDRefl::Mngr.AddField<&StencilState::readMask>("readMask");
  MyDRefl::Mngr.AddField<&StencilState::writeMask>("writeMask");
  MyDRefl::Mngr.AddField<&StencilState::failOp>("failOp");
  MyDRefl::Mngr.AddField<&StencilState::depthFailOp>("depthFailOp");
  MyDRefl::Mngr.AddField<&StencilState::passOp>("passOp");
  MyDRefl::Mngr.AddField<&StencilState::func>("func");
}

