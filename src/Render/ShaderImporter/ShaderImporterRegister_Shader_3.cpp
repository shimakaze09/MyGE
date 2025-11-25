#include <MyGE/Render/Shader.h>

#include "ShaderImporterRegisterImpl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::ShaderImporterRegister_Shader_3() {
  MyDRefl::Mngr.AddField<&Shader::passes>("passes");
}

