//
// Created by Admin on 16/03/2025.
//

#pragma once

namespace My::MyGE {
// singleton
struct WorldTime {
  double elapsedTime;  // in seconds
  float deltaTime;     // in seconds
};
}  // namespace My::MyGE

#include "details/WorldTime_AutoRefl.inl"
