#include <MyGE/Render/RootParameter.h>

#include "ShaderImporterRegisterImpl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::ShaderImporterRegister_RootDescriptorType() {
  MyDRefl::Mngr.RegisterType<RootDescriptorType>();
  MyDRefl::Mngr.SimpleAddField<RootDescriptorType::SRV>("SRV");
  MyDRefl::Mngr.SimpleAddField<RootDescriptorType::UAV>("UAV");
  MyDRefl::Mngr.SimpleAddField<RootDescriptorType::CBV>("CBV");
}
