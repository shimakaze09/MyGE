#pragma once

#include <MyGE/Render/Shader.h>

namespace My::MyGE {
class ShaderCompiler {
 public:
  static ShaderCompiler& Instance() noexcept {
    static ShaderCompiler instance;
    return instance;
  }

  std::tuple<bool, Shader> Compile(std::string_view ushader);

 private:
  ShaderCompiler();
  ~ShaderCompiler();

  struct Impl;
  Impl* pImpl;
};
}  // namespace My::MyGE
