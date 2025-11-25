#include <MyGE/Render/RenderState.h>

#include "ShaderImporterRegisterImpl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::ShaderImporterRegister_FillMode() {
  MyDRefl::Mngr.RegisterType<FillMode>();
  MyDRefl::Mngr.AddField<FillMode::WIREFRAME>("WIREFRAME");
  MyDRefl::Mngr.AddField<FillMode::SOLID>("SOLID");
}
