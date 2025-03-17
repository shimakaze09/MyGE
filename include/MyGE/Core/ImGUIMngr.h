//
// Created by Admin on 17/03/2025.
//

#pragma once

#include <MyDX12/MyDX12.h>

namespace My::MyGE {
class ImGUIMngr {
 public:
  static ImGUIMngr& Instance() noexcept {
    static ImGUIMngr instance;
    return instance;
  }

  enum class Style { Dark, Classic };

  void Init(HWND, ID3D12Device*, size_t numFrame, Style = Style::Dark);
  void Clear();

 private:
  My::MyDX12::DescriptorHeapAllocation fontDH;
};
}  // namespace My::MyGE
