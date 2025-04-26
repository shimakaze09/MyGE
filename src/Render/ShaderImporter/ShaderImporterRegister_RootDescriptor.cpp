#include <MyGE/Render/RootParameter.h>

#include "ShaderImporterRegisterImpl.h"

using namespace Smkz::MyGE;
using namespace Smkz::MyDRefl;

void Smkz::MyGE::details::ShaderImporterRegister_RootDescriptor() {
  MyDRefl::Mngr.RegisterType<RootDescriptor>();
  MyDRefl::Mngr.SimpleAddField<&RootDescriptor::DescriptorType>(
      "DescriptorType");
  MyDRefl::Mngr.SimpleAddField<&RootDescriptor::ShaderRegister>(
      "ShaderRegister");
  MyDRefl::Mngr.SimpleAddField<&RootDescriptor::RegisterSpace>("RegisterSpace");
}
