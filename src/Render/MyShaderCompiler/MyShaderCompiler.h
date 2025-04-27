#pragma once

#include <MyGE/Render/Shader.h>

namespace Smkz::MyGE {
class MyShaderCompiler {
 public:
  static MyShaderCompiler& Instance() noexcept {
    static MyShaderCompiler instance;
    return instance;
  }

  std::tuple<bool, Shader> Compile(std::string_view myshader);

 private:
  MyShaderCompiler();
  ~MyShaderCompiler();

  struct Impl;
  Impl* pImpl;
};
}  // namespace Smkz::MyGE
