//
// Created by Admin on 16/03/2025.
//

#include <MyGE/Asset/AssetMngr.h>
#include <MyGE/Core/Image.h>
#include <MyGE/Core/Texture2D.h>

#include <iostream>

using namespace My::MyGE;
using namespace My;

int main() {
  std::filesystem::path imgPath = "../assets/textures/test.png";
  std::filesystem::path tex2dPath = "../assets/textures/test.tex2d";

  if (!std::filesystem::is_directory("../assets/textures"))
    std::filesystem::create_directories("../assets/textures");

  {  // create image
    size_t width = 20;
    size_t height = 10;
    Image img(width, height, 3);
    for (size_t j = 0; j < height; j++) {
      for (size_t i = 0; i < width; i++)
        img.At<rgbf>(i, j) = {i / (float)width, j / (float)height, 0.f};
    }
    img.Save(imgPath.string());
  }

  {
    auto img = AssetMngr::Instance().LoadAsset<Image>(imgPath);

    auto tex2d = new Texture2D;
    tex2d->image = img;
    tex2d->wrapMode = Texture2D::WrapMode::Clamp;
    tex2d->filterMode = Texture2D::FilterMode::Point;

    AssetMngr::Instance().CreateAsset(tex2d, tex2dPath);
    AssetMngr::Instance().Clear();
  }

  AssetMngr::Instance().ImportAsset(imgPath);
  auto tex2d = AssetMngr::Instance().LoadAsset<Texture2D>(tex2dPath);
  for (size_t j = 0; j < tex2d->image->height; j++) {
    for (size_t i = 0; i < tex2d->image->width; i++)
      std::cout << tex2d->image->At<rgbf>(i, j) << std::endl;
  }
  std::cout << static_cast<size_t>(tex2d->wrapMode) << std::endl;
  std::cout << static_cast<size_t>(tex2d->filterMode) << std::endl;

  return 0;
}
