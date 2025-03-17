//
// Created by Admin on 16/03/2025.
//

#pragma once

#include "IPipeline.h"

namespace My::MyGE {
class StdPipeline final : public IPipeline {
 public:
  StdPipeline(InitDesc desc);
  virtual ~StdPipeline();

  virtual void UpdateRenderContext(const MyECS::World&) override;

  virtual void Render(ID3D12Resource* curBackBuffer) override;
  virtual void EndFrame() override;

 protected:
  virtual void Impl_Resize() override;

 private:
  struct Impl;
  Impl* pImpl;
};
}  // namespace My::MyGE
