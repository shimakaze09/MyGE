#include <MyGE/Render/RootParameter.h>

#include "ShaderImporterRegisterImpl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::ShaderImporterRegister_RootDescriptorType() {
  MyDRefl::Mngr.RegisterType<RootDescriptorType>();
  MyDRefl::Mngr.AddField<RootDescriptorType::SRV>("SRV");
  MyDRefl::Mngr.AddField<RootDescriptorType::UAV>("UAV");
  MyDRefl::Mngr.AddField<RootDescriptorType::CBV>("CBV");
}

