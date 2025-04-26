#include <MyGE/Render/RenderState.h>

#include "ShaderImporterRegisterImpl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::ShaderImporterRegister_CompareFunc() {
  MyDRefl::Mngr.RegisterType<CompareFunc>();
  MyDRefl::Mngr.SimpleAddField<CompareFunc::NEVER>("NEVER");
  MyDRefl::Mngr.SimpleAddField<CompareFunc::LESS>("LESS");
  MyDRefl::Mngr.SimpleAddField<CompareFunc::EQUAL>("EQUAL");
  MyDRefl::Mngr.SimpleAddField<CompareFunc::LESS_EQUAL>("LESS_EQUAL");
  MyDRefl::Mngr.SimpleAddField<CompareFunc::GREATER>("GREATER");
  MyDRefl::Mngr.SimpleAddField<CompareFunc::NOT_EQUAL>("NOT_EQUAL");
  MyDRefl::Mngr.SimpleAddField<CompareFunc::GREATER_EQUAL>("GREATER_EQUAL");
  MyDRefl::Mngr.SimpleAddField<CompareFunc::ALWAYS>("ALWAYS");
}
