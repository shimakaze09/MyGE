#include <MyGE/Render/RenderState.h>

#include "ShaderImporterRegisterImpl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::ShaderImporterRegister_StencilOp() {
  MyDRefl::Mngr.RegisterType<StencilOp>();
  MyDRefl::Mngr.AddField<StencilOp::KEEP>("KEEP");
  MyDRefl::Mngr.AddField<StencilOp::ZERO>("ZERO");
  MyDRefl::Mngr.AddField<StencilOp::REPLACE>("REPLACE");
  MyDRefl::Mngr.AddField<StencilOp::INCR_SAT>("INCR_SAT");
  MyDRefl::Mngr.AddField<StencilOp::DECR_SAT>("DECR_SAT");
  MyDRefl::Mngr.AddField<StencilOp::INVERT>("INVERT");
  MyDRefl::Mngr.AddField<StencilOp::INCR>("INCR");
  MyDRefl::Mngr.AddField<StencilOp::DECR>("DECR");
}
