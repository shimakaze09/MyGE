#pragma once

#include <MyGE/Render/Shader.h>

namespace My::MyGE {
class MyShaderCompiler {
 public:
  static MyShaderCompiler& Instance() noexcept {
    static MyShaderCompiler instance;
    return instance;
  }

  std::tuple<bool, Shader> Compile(std::string_view ushader);

 private:
  MyShaderCompiler();
  ~MyShaderCompiler();

  struct Impl;
  Impl* pImpl;
};
}  // namespace My::MyGE
