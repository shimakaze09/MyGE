#include <MyGE/Render/Shader.h>

#include "ShaderImporterRegisterImpl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::ShaderImporterRegister_Shader() {
  MyDRefl::Mngr.RegisterType<Shader>();
  MyDRefl::Mngr.AddField<&Shader::hlslFile>("hlslFile");
  MyDRefl::Mngr.AddField<&Shader::name>("name");
}
