#include <MyGE/Render/RenderState.h>

#include "ShaderImporterRegisterImpl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::ShaderImporterRegister_StencilState() {
  MyDRefl::Mngr.RegisterType<StencilState>();
  MyDRefl::Mngr.SimpleAddField<&StencilState::enable>("enable");
  MyDRefl::Mngr.SimpleAddField<&StencilState::ref>("ref");
  MyDRefl::Mngr.SimpleAddField<&StencilState::readMask>("readMask");
  MyDRefl::Mngr.SimpleAddField<&StencilState::writeMask>("writeMask");
  MyDRefl::Mngr.SimpleAddField<&StencilState::failOp>("failOp");
  MyDRefl::Mngr.SimpleAddField<&StencilState::depthFailOp>("depthFailOp");
  MyDRefl::Mngr.SimpleAddField<&StencilState::passOp>("passOp");
  MyDRefl::Mngr.SimpleAddField<&StencilState::func>("func");
}
