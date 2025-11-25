#include <MyGE/Render/RootParameter.h>

#include "ShaderImporterRegisterImpl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::ShaderImporterRegister_RootConstants() {
  MyDRefl::Mngr.RegisterType<RootConstants>();
  MyDRefl::Mngr.AddField<&RootConstants::ShaderRegister>(
      "ShaderRegister");
  MyDRefl::Mngr.AddField<&RootConstants::RegisterSpace>("RegisterSpace");
  MyDRefl::Mngr.AddField<&RootConstants::Num32BitValues>(
      "Num32BitValues");
}

