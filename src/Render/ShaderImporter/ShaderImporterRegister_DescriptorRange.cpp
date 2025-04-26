#include <MyGE/Render/RootParameter.h>

#include "ShaderImporterRegisterImpl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::ShaderImporterRegister_DescriptorRange() {
  MyDRefl::Mngr.RegisterType<DescriptorRange>();
  MyDRefl::Mngr.SimpleAddField<&DescriptorRange::RangeType>("RangeType");
  MyDRefl::Mngr.SimpleAddField<&DescriptorRange::NumDescriptors>(
      "NumDescriptors");
  MyDRefl::Mngr.SimpleAddField<&DescriptorRange::BaseShaderRegister>(
      "BaseShaderRegister");
  MyDRefl::Mngr.SimpleAddField<&DescriptorRange::RegisterSpace>(
      "RegisterSpace");
}
