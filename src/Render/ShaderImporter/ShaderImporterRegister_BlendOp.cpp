#include <MyGE/Render/RenderState.h>

#include "ShaderImporterRegisterImpl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::ShaderImporterRegister_BlendOp() {
  MyDRefl::Mngr.RegisterType<BlendOp>();
  MyDRefl::Mngr.SimpleAddField<BlendOp::ADD>("ADD");
  MyDRefl::Mngr.SimpleAddField<BlendOp::SUBTRACT>("SUBTRACT");
  MyDRefl::Mngr.SimpleAddField<BlendOp::REV_SUBTRACT>("REV_SUBTRACT");
  MyDRefl::Mngr.SimpleAddField<BlendOp::MIN>("MIN");
  MyDRefl::Mngr.SimpleAddField<BlendOp::MAX>("MAX");
}
