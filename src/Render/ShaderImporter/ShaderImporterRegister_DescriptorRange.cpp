#include <MyGE/Render/RootParameter.h>

#include "ShaderImporterRegisterImpl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::ShaderImporterRegister_DescriptorRange() {
  MyDRefl::Mngr.RegisterType<DescriptorRange>();
  MyDRefl::Mngr.AddField<&DescriptorRange::RangeType>("RangeType");
  MyDRefl::Mngr.AddField<&DescriptorRange::NumDescriptors>(
      "NumDescriptors");
  MyDRefl::Mngr.AddField<&DescriptorRange::BaseShaderRegister>(
      "BaseShaderRegister");
  MyDRefl::Mngr.AddField<&DescriptorRange::RegisterSpace>(
      "RegisterSpace");
}

