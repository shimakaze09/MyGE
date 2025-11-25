#include <MyGE/Render/RenderState.h>

#include "ShaderImporterRegisterImpl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::ShaderImporterRegister_CompareFunc() {
  MyDRefl::Mngr.RegisterType<CompareFunc>();
  MyDRefl::Mngr.AddField<CompareFunc::NEVER>("NEVER");
  MyDRefl::Mngr.AddField<CompareFunc::LESS>("LESS");
  MyDRefl::Mngr.AddField<CompareFunc::EQUAL>("EQUAL");
  MyDRefl::Mngr.AddField<CompareFunc::LESS_EQUAL>("LESS_EQUAL");
  MyDRefl::Mngr.AddField<CompareFunc::GREATER>("GREATER");
  MyDRefl::Mngr.AddField<CompareFunc::NOT_EQUAL>("NOT_EQUAL");
  MyDRefl::Mngr.AddField<CompareFunc::GREATER_EQUAL>("GREATER_EQUAL");
  MyDRefl::Mngr.AddField<CompareFunc::ALWAYS>("ALWAYS");
}

