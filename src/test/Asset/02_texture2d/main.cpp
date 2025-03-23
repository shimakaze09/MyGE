#include <MyGE/Asset/AssetMngr.h>
#include <MyGE/Core/Image.h>
#include <MyGE/Render/Texture2D.h>

#include <iostream>

using namespace My::MyGE;
using namespace My;

int main() {
  // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

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

    Texture2D tex2d;
    tex2d.image = img;

    AssetMngr::Instance().CreateAsset(tex2d, tex2dPath);
    AssetMngr::Instance().Clear();
  }

  AssetMngr::Instance().ImportAsset(imgPath);
  auto tex2d = AssetMngr::Instance().LoadAsset<Texture2D>(tex2dPath);
  for (size_t j = 0; j < tex2d->image->height; j++) {
    for (size_t i = 0; i < tex2d->image->width; i++)
      std::cout << tex2d->image->At<rgbf>(i, j) << std::endl;
  }

  return 0;
}
