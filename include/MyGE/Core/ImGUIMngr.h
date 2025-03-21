#pragma once

#include <MyDX12/MyDX12.h>

#include <vector>

struct ID3D12Device;
struct ImFontAtlas;
struct ImGuiContext;
struct ImGuiIO;

namespace My::MyGE {
class ImGUIMngr {
 public:
  static ImGUIMngr& Instance() noexcept {
    static ImGUIMngr instance;
    return instance;
  }

  enum class StyleColors {
    Classic,
    Dark,
    Light,
  };

  void Init(void* hwnd, ID3D12Device*, size_t numFrames, size_t numContexts,
            StyleColors = StyleColors::Dark);
  const std::vector<ImGuiContext*>& GetContexts() const;

  void Clear();

 private:
  struct Impl;
  Impl* pImpl;

  ImGUIMngr();
  ~ImGUIMngr();
};
}  // namespace My::MyGE
