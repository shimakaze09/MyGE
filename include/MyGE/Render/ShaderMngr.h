#pragma once

#include <map>
#include <string>

namespace My::MyGE {
struct Shader;

class ShaderMngr {
 public:
  static ShaderMngr& Instance() noexcept {
    static ShaderMngr instance;
    return instance;
  }

  void Register(Shader*);
  Shader* Get(std::string_view name) const;

  const std::map<std::string, Shader*, std::less<>> GetShaderMap()
      const noexcept {
    return shaderMap;
  }

 private:
  ShaderMngr() = default;
  std::map<std::string, Shader*, std::less<>> shaderMap;
};
}  // namespace My::MyGE
