#include <MyGE/Render/RenderState.h>

#include "ShaderImporterRegisterImpl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::ShaderImporterRegister_StencilOp() {
  MyDRefl::Mngr.RegisterType<StencilOp>();
  MyDRefl::Mngr.SimpleAddField<StencilOp::KEEP>("KEEP");
  MyDRefl::Mngr.SimpleAddField<StencilOp::ZERO>("ZERO");
  MyDRefl::Mngr.SimpleAddField<StencilOp::REPLACE>("REPLACE");
  MyDRefl::Mngr.SimpleAddField<StencilOp::INCR_SAT>("INCR_SAT");
  MyDRefl::Mngr.SimpleAddField<StencilOp::DECR_SAT>("DECR_SAT");
  MyDRefl::Mngr.SimpleAddField<StencilOp::INVERT>("INVERT");
  MyDRefl::Mngr.SimpleAddField<StencilOp::INCR>("INCR");
  MyDRefl::Mngr.SimpleAddField<StencilOp::DECR>("DECR");
}
