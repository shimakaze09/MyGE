//
// Created by Admin on 13/03/2025.
//

#pragma once

#include <MyECS/Entity.h>

#include <vector>

namespace My::DustEngine {
struct Parent {
  UECS::Entity value{UECS::Entity::Invalid()};
};
}  // namespace Ubpa::DustEngine

#include "details/Parent_AutoRefl.inl"
