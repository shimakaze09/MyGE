#include <MyGE/Render/RootParameter.h>

#include "ShaderImporterRegisterImpl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::ShaderImporterRegister_RootConstants() {
  MyDRefl::Mngr.RegisterType<RootConstants>();
  MyDRefl::Mngr.SimpleAddField<&RootConstants::ShaderRegister>(
      "ShaderRegister");
  MyDRefl::Mngr.SimpleAddField<&RootConstants::RegisterSpace>("RegisterSpace");
  MyDRefl::Mngr.SimpleAddField<&RootConstants::Num32BitValues>(
      "Num32BitValues");
}
