#pragma once

namespace My::MyGE {
// Topology of Mesh faces.
enum class MeshTopology {
  Triangles,  // Mesh is made from triangles.
  // Quads, // Mesh is made from quads.
  Lines,      // Mesh is made from lines.
  LineStrip,  // Mesh is a line strip.
  Points      // Mesh is made from points.
};
}  // namespace My::MyGE

