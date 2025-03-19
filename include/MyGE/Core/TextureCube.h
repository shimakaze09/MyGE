//
// Created by Admin on 19/03/2025.
//

#pragma once
 
 #include "Texture.h"
 
 #include <array>
 
 namespace My::MyGE {
 class Image;
 
 struct TextureCube : Texture {
   std::array<const Image*, 6> images;
 };
 }
