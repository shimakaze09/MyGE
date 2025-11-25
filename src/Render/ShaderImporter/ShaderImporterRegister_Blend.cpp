#include <MyGE/Render/RenderState.h>

#include "ShaderImporterRegisterImpl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::ShaderImporterRegister_Blend() {
  MyDRefl::Mngr.RegisterType<Blend>();
  MyDRefl::Mngr.AddField<Blend::ZERO>("ZERO");
  MyDRefl::Mngr.AddField<Blend::ONE>("ONE");
  MyDRefl::Mngr.AddField<Blend::SRC_COLOR>("SRC_COLOR");
  MyDRefl::Mngr.AddField<Blend::INV_SRC_COLOR>("INV_SRC_COLOR");
  MyDRefl::Mngr.AddField<Blend::SRC_ALPHA>("SRC_ALPHA");
  MyDRefl::Mngr.AddField<Blend::INV_SRC_ALPHA>("INV_SRC_ALPHA");
  MyDRefl::Mngr.AddField<Blend::DEST_ALPHA>("DEST_ALPHA");
  MyDRefl::Mngr.AddField<Blend::INV_DEST_ALPHA>("INV_DEST_ALPHA");
  MyDRefl::Mngr.AddField<Blend::DEST_COLOR>("DEST_COLOR");
  MyDRefl::Mngr.AddField<Blend::INV_DEST_COLOR>("INV_DEST_COLOR");
  MyDRefl::Mngr.AddField<Blend::SRC_ALPHA_SAT>("SRC_ALPHA_SAT");
  MyDRefl::Mngr.AddField<Blend::BLEND_FACTOR>("BLEND_FACTOR");
  MyDRefl::Mngr.AddField<Blend::INV_BLEND_FACTOR>("INV_BLEND_FACTOR");
  MyDRefl::Mngr.AddField<Blend::SRC1_COLOR>("SRC1_COLOR");
  MyDRefl::Mngr.AddField<Blend::INV_SRC1_COLOR>("INV_SRC1_COLOR");
  MyDRefl::Mngr.AddField<Blend::SRC1_ALPHA>("SRC1_ALPHA");
  MyDRefl::Mngr.AddField<Blend::INV_SRC1_ALPHA>("INV_SRC1_ALPHA");
}
