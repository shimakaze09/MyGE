#pragma once
 
#include <MyGM/scale.h>
 
namespace My::MyGE {
struct NonUniformScale {
  scalef3 value{ 1.f };
};
}
 
#include "details/NonUniformScale_AutoRefl.inl"
