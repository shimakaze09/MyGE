#include <MyGE/Render/Shader.h>

#include "ShaderImporterRegisterImpl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::ShaderImporterRegister_Shader_1() {
  MyDRefl::Mngr.AddField<&Shader::rootParameters>("rootParameters");
}
