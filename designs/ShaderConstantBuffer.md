# Shader Constant Buffer

Each shader requires some constant buffers.

Assuming they change every frame.

The engine provides each shader with any number of manually resizable constant buffers.

The provisional usage strategy is as follows:

- Frame resources provide a dedicated constant buffer for each shader
- The constant buffer will be filled with per camera, per material, and per object data
- Each frame first counts the number of materials involved in each shader and the number of objects for each material,
  calculating the size of the constant buffer, then fills in the data (recalculating everything every frame)

> **Example**
>
> Assume:
>
> - 2 shaders
> - Each shader has 2 materials
> - Each material has 2 objects
> - 2 cameras
>
> > Naming and sizes:
> >
> > - shader0
      > >
- material00 (size_0_m)
  > >
- object000 (size_0_o)
> >     - object001 (size_0_o)
> >   - material01 (size_0_m)
        > >
- object010 (size_0_o)
> >     - object011 (size_0_o)
> > - shader1
      > >
- material10 (size_1_m)
  > >
- object100 (size_1_o)
> >     - object101 (size_1_o)
> >   - material11 (size_1_m)
        > >
- object110 (size_1_o)
> >     - object111 (size_1_o)
> > - camera0, camera1
>
> The constant buffer layout would be:
>
> ```
> [shader0 constant buffer]
> camera0
> camera1
> material00
> material01
> object000
> object001
> object010
> object011
> 
> [shader1 constant buffer]
> camera0
> camera1
> material10
> material11
> object100
> object101
> object110
> object111
> ```

The constant buffer used by shaders
is [MyDX12::DynamicConstantBuffer](https://github.com/shimakaze09/MyDX12/tree/main/include/MyDX12).

Frame resources include [ShaderCBMngrDX12](../include/MyGE/Render/DX12/ShaderCBMngrDX12.h).

A shared buffer between multiple shaders may be considered in the future.