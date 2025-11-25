#include <MyGE/Render/Shader.h>

#include "ShaderImporterRegisterImpl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::ShaderImporterRegister_Shader_1() {
  MyDRefl::Mngr.AddField<&Shader::rootParameters>("rootParameters");
}

