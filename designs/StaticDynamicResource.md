# Static/Dynamic Resources

Data is divided into two categories:

- Small (per shader): constant buffer
- Large: mesh, texture

What are static and dynamic resources?

Static resources don't change, while dynamic resources may change every frame.

Per shader resources are by default dynamic, changing every frame. This is covered
in [Shader Constant Buffer](ShaderConstantBuffer.md).

Meshes and textures are more resource-intensive and require more detailed consideration.

Since meshes and textures are similar, they'll be explained together without distinction.

Two boolean flags are used to mark resource states:

- dirty: whether modifications occurred in the current frame
- editable: whether the resource can be modified

[D3D12_HEAP_TYPE enumeration](https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ne-d3d12-d3d12_heap_type)
describes the usage of upload heaps and default heaps.

Static resources are placed in the default heap.

For dynamic resources, if they only need to be read once, they're placed only in the upload heap. If they need to be
read multiple times, they need to be copied to the default heap.

For shader resources like per pass, per material, and per object constant buffers, they only need to be placed in the upload heap (as they are recalculated every frame).

For meshes, since they're likely to be used multiple times (shadow pass + geometry pass), they're copied to the upload
heap by default.

There are several scenarios:

- Unregistered GPU resources
    - editable: placed in the default heap (upload heap is released), converted to static
    - non-editable: placed in the upload heap and transferred to the default heap, upload heap resources not released,
      converted to dynamic
- Dynamic
    - non-editable
        - dirty: update upload heap resources and transfer to default heap, upload heap resources are **released**,
          converted to static
        - non-dirty: release upload heap resources, converted to static
    - editable
        - dirty: update upload heap resources and transfer to default heap, upload heap resources not released, remain
          dynamic
        - non-dirty: remain dynamic
- Static: remain static
