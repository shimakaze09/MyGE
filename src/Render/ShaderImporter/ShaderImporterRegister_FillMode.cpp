#include <MyGE/Render/RenderState.h>

#include "ShaderImporterRegisterImpl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::ShaderImporterRegister_FillMode() {
  MyDRefl::Mngr.RegisterType<FillMode>();
  MyDRefl::Mngr.SimpleAddField<FillMode::WIREFRAME>("WIREFRAME");
  MyDRefl::Mngr.SimpleAddField<FillMode::SOLID>("SOLID");
}
