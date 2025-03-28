#include <MyGE/Core/Image.h>
#include <MyGE/Render/TextureCube.h>
#include <MyGM/vec.h>

#include <cassert>
#include <thread>

using namespace My::MyGE;
using namespace My;

TextureCube::TextureCube(std::array<std::shared_ptr<const Image>, 6> images) {
  Init(images);
}

TextureCube::TextureCube(std::shared_ptr<const Image> equirectangularMap) {
  Init(equirectangularMap);
}

void TextureCube::Init(std::array<std::shared_ptr<const Image>, 6> images) {
  Clear();
  mode = SourceMode::SixSidedImages;
  for (size_t i = 0; i < 6; i++)
    this->images[i] = images[i];
}

void TextureCube::Init(std::shared_ptr<const Image> equirectangularMap) {
  Clear();
  mode = SourceMode::EquirectangularMap;
#ifdef _DEBUG
  size_t s = equirectangularMap->height / 2;
#else
  size_t s = equirectangularMap->height;
#endif
  size_t c = equirectangularMap->channel;

  vecf3 origin[6] = {
      {1, -1, -1},   // left   +x
      {-1, -1, 1},   // right  -x
      {-1, -1, 1},   // top    +y
      {-1, 1, -1},   // bottom -y
      {-1, -1, -1},  // back   +z
      {1, -1, 1},    // front  -z
  };

  vecf3 right[6] = {
      {0, 0, 2},   // left   +x
      {0, 0, -2},  // right  -x
      {2, 0, 0},   // top    +y
      {2, 0, 0},   // bottom -y
      {2, 0, 0},   // back   +z
      {-2, 0, 0},  // front  -z
  };

  vecf3 up[6] = {
      {0, 2, 0},   // left   +x
      {0, 2, 0},   // right  -x
      {0, 0, -2},  // top    +y
      {0, 0, 2},   // bottom -y
      {0, 2, 0},   // back   +z
      {0, 2, 0},   // front  -z
  };

  std::array<std::shared_ptr<Image>, 6> imgs;
  for (size_t i = 0; i < 6; i++)
    images[i] = imgs[i] = std::make_shared<Image>(s, s, c);

  size_t N = std::thread::hardware_concurrency();
  auto work = [&](size_t id) {
    for (size_t i = 0; i < 6; i++) {
      auto img = imgs[i];
      for (size_t y = id; y < s; y += N) {
        for (size_t x = 0; x < s; x++) {
          vecf3 p =
              origin[i] + (x / float(s)) * right[i] + (y / float(s)) * up[i];
          p.normalize_self();
          static constexpr vecf2 invAtan = {0.1591f, 0.3183f};
          pointf2 uv = {std::atan2(p[2], p[0]), std::asin(p[1])};
          uv[0] = 0.5f - uv[0] * invAtan[0];
          uv[1] = 0.5f + uv[1] * invAtan[1];
          auto color = equirectangularMap->SampleLinear(uv);
          for (size_t k = 0; k < c; k++)
            img->At(x, y, k) = color[k];
        }
      }
    }
  };

  std::vector<std::thread> workers;
  for (size_t i = 0; i < std::thread::hardware_concurrency(); i++)
    workers.emplace_back(work, i);

  for (auto& worker : workers)
    worker.join();
}

void TextureCube::Clear() {
  for (auto& img : images.val)
    img.reset();
  equirectangularMap.val.reset();
}
