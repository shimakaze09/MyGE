#include <MyGE/Render/RootParameter.h>

#include "ShaderImporterRegisterImpl.h"

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::details::ShaderImporterRegister_RootDescriptor() {
  MyDRefl::Mngr.RegisterType<RootDescriptor>();
  MyDRefl::Mngr.AddField<&RootDescriptor::DescriptorType>("DescriptorType");
  MyDRefl::Mngr.AddField<&RootDescriptor::ShaderRegister>("ShaderRegister");
  MyDRefl::Mngr.AddField<&RootDescriptor::RegisterSpace>("RegisterSpace");
}
