//
// Created by Admin on 19/03/2025.
//

#pragma once
 
 #include "../TextureCube.h"
 
 namespace My::MyGE {
 // singleton
 struct Skybox {
   const TextureCube* texcube;
 };
 }
 
#include "details/Skybox_AutoRefl.inl"
