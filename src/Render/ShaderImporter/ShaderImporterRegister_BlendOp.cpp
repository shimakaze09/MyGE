#include <MyGE/Render/RenderState.h>

#include "ShaderImporterRegisterImpl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::ShaderImporterRegister_BlendOp() {
  MyDRefl::Mngr.RegisterType<BlendOp>();
  MyDRefl::Mngr.AddField<BlendOp::ADD>("ADD");
  MyDRefl::Mngr.AddField<BlendOp::SUBTRACT>("SUBTRACT");
  MyDRefl::Mngr.AddField<BlendOp::REV_SUBTRACT>("REV_SUBTRACT");
  MyDRefl::Mngr.AddField<BlendOp::MIN>("MIN");
  MyDRefl::Mngr.AddField<BlendOp::MAX>("MAX");
}
