//
// Created by Admin on 15/03/2025.
//

#pragma once

#include <string>

namespace My::MyGE {
class HLSLFile {
 public:
  HLSLFile(std::string str, std::string localDir)
      : str{std::move(str)}, localDir{std::move(localDir)} {}

  const std::string& GetString() const noexcept { return str; }

  const std::string& GetLocalDir() const noexcept { return localDir; }

 private:
  std::string str;
  std::string localDir;
};
}  // namespace My::MyGE
