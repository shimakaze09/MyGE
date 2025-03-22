#pragma once

#include <_deps/crossguid/guid.hpp>

namespace My::MyGE {
struct ProjectViewer {
  xg::Guid selectedFolder;
  xg::Guid selectedAsset;
};
}  // namespace My::MyGE
