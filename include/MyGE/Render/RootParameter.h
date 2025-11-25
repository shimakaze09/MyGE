#pragma once

#include <MyTemplate/Name.hpp>
#include <compare>
#include <variant>
#include <vector>

namespace My::MyGE {
enum class RootDescriptorType {
  SRV,
  UAV,
  CBV,
};

struct DescriptorRange {
  RootDescriptorType RangeType;
  unsigned int NumDescriptors;
  unsigned int BaseShaderRegister;
  unsigned int RegisterSpace;

  auto operator<=>(const DescriptorRange&) const = default;

  void Init(RootDescriptorType RangeType, unsigned int NumDescriptors,
            unsigned int BaseShaderRegister, unsigned int RegisterSpace = 0) {
    this->RangeType = RangeType;
    this->NumDescriptors = NumDescriptors;
    this->BaseShaderRegister = BaseShaderRegister;
    this->RegisterSpace = RegisterSpace;
  }
};

using RootDescriptorTable = std::vector<DescriptorRange>;

struct RootConstants {
  unsigned int ShaderRegister;
  unsigned int RegisterSpace;
  unsigned int Num32BitValues;

  auto operator<=>(const RootConstants&) const = default;
};

struct RootDescriptor {
  RootDescriptorType DescriptorType{};  // ignore sampler
  unsigned int ShaderRegister{0};
  unsigned int RegisterSpace{0};

  auto operator<=>(const RootDescriptor&) const = default;

  void Init(RootDescriptorType DescriptorType, unsigned int ShaderRegister,
            unsigned int RegisterSpace = 0) {
    this->DescriptorType = DescriptorType;
    this->ShaderRegister = ShaderRegister;
    this->RegisterSpace = RegisterSpace;
  }
};

using RootParameter =
    std::variant<RootDescriptorTable, RootConstants, RootDescriptor>;
}  // namespace My::MyGE

template <>
constexpr auto My::type_name<My::MyGE::RootParameter>() noexcept {
  return TSTR("My::MyGE::RootParameter");
}
