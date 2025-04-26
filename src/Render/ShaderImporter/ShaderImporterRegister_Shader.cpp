#include <MyGE/Render/Shader.h>

#include "ShaderImporterRegisterImpl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::ShaderImporterRegister_Shader() {
  MyDRefl::Mngr.RegisterType<Shader>();
  MyDRefl::Mngr.AddField<&Shader::hlslFile>("hlslFile");
  MyDRefl::Mngr.SimpleAddField<&Shader::name>("name");
}
