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

  // data : cpu -> gpu
  // run in update
  virtual void BeginFrame(const MyECS::World&) override;

  // run in draw
  virtual void Render(ID3D12Resource* rt) override;
 
  // run at the end of draw
  virtual void EndFrame() override;

 protected:
  virtual void Impl_Resize() override;

 private:
  struct Impl;
  Impl* pImpl;
};
}  // namespace My::MyGE
