//
// Created by Admin on 15/03/2025.
//

#pragma once

#include <MyGM/MyGM.h>

#include <vector>

#include "Object.h"
#include "SubMeshDescriptor.h"

namespace My::MyGE {
class Mesh : public Object {
 public:
  Mesh(bool isEditable = true) : isEditable{isEditable} {}

  const std::vector<pointf3>& GetPositions() const noexcept {
    return positions;
  }

  const std::vector<pointf2>& GetUV() const noexcept { return uv; }

  const std::vector<normalf>& GetNormals() const noexcept { return normals; }

  const std::vector<vecf3>& GetTangents() const noexcept { return tangents; }

  const std::vector<rgbf>& GetColors() const noexcept { return colors; }

  const std::vector<uint32_t>& GetIndices() const noexcept { return indices; }

  const std::vector<SubMeshDescriptor>& GetSubMeshes() const noexcept {
    return submeshes;
  }

  // must editable
  void SetPositions(std::vector<pointf3> positions);
  void SetUV(std::vector<pointf2> uv);
  void SetNormals(std::vector<normalf> normals);
  void SetTangents(std::vector<vecf3> tangents);
  void SetColors(std::vector<rgbf> colors);
  void SetIndices(std::vector<uint32_t> indices);
  void SetSubMeshCount(size_t num);
  void SetSubMesh(size_t index, SubMeshDescriptor desc);

  bool IsDirty() const noexcept { return dirty; }

  bool IsEditable() const noexcept { return isEditable; }

  const void* GetVertexBufferData() const noexcept {
    return vertexBuffer.data();
  }

  size_t GetVertexBufferVertexCount() const noexcept {
    return positions.size();
  }

  size_t GetVertexBufferVertexStride() const noexcept {
    return vertexBuffer.size() / positions.size();
  }

  // if dirty, update vertex buffer and set non dirty
  // call by the pipeline, need to update GPU buffer
  void UpdateVertexBuffer(bool setToNonEditable = false);

  // non empty and every attributes have same num
  bool IsVertexValid();

 private:
  std::vector<pointf3> positions;
  std::vector<pointf2> uv;
  std::vector<normalf> normals;
  std::vector<vecf3> tangents;
  std::vector<rgbf> colors;
  std::vector<uint32_t> indices;
  std::vector<SubMeshDescriptor> submeshes;

  // pos, uv, normal, tangent, color
  std::vector<uint8_t> vertexBuffer;

  bool isEditable;
  bool dirty{false};
};
}  // namespace My::MyGE
