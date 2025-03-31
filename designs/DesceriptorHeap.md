# Managing Descriptor Heaps

DX12 resource binding is divided into two stages, with descriptors (SRV, UAV, CBV, RTV, DSV, Sampler) serving as
intermediaries.

![D3D12ResourceBinding](http://diligentgraphics.com/wp-content/uploads/2016/03/D3D12ResourceBinding-1024x807.png)

There are four types of descriptor heaps in DX12:

- CBV / SRV / UAV (`D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV`)
- Sampler (`D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER`)
- RTV (`D3D12_DESCRIPTOR_HEAP_TYPE_RTV`)
- DSV (`D3D12_DESCRIPTOR_HEAP_TYPE_DSV`)

If you want GPU access, the heap must be marked as **shader-visible**. Only the first two types can be marked as
shader-visible, while RTV and DSV can only be CPU-visible.

Shader-visible descriptor heap sizes are limited, so not all descriptors can exist in the GPU. Applications need to
ensure that descriptors required for rendering are available in the GPU.

## Overview

The descriptor heap management system primarily uses 6 classes:

- `VarSizeAllocMngr`: Implementation class for allocation-free mechanism
- `DescriptorHeapAllocation`: A single allocation from the descriptor heap
- `DescriptorHeapAllocationManager`: Descriptor heap allocation manager, a specialization of `VarSizeAllocMngr`
- `CPUDescriptorHeap`: CPU-only descriptor heap group, containing multiple `DescriptorHeapAllocationManager` to
  implement expandable descriptor allocation
- `DynamicSuballocationsManager`: Shader-visible descriptor heap allocation manager, a specialization of
  `VarSizeAllocMngr`. Unlike `DescriptorHeapAllocationManager`, it doesn't immediately release on `Free`, but batches
  releases by calling `ReleaseAllocations` at the end of the frame
- `GPUDescriptorHeap`: Shader-visible descriptor heap, containing 2 `DescriptorHeapAllocationManager` instances - one
  for static allocation and one for dynamic allocation. For dynamic allocation, each thread gets its own
  `DynamicSuballocationsManager` to enable lock-free operation. Note that it's not expandable, so timely release is
  necessary.

## VarSizeAllocMngr

![FreeBlockListManager1](http://diligentgraphics.com/wp-content/uploads/2017/04/FreeBlockListManager1.png)

## DescriptorHeapAllocation

![DescriptorHeapAllocation](http://diligentgraphics.com/wp-content/uploads/2017/04/DescriptorHeapAllocation.png)

## DescriptorHeapAllocationManager

![DescriptorHeapAllocationsManager](http://diligentgraphics.com/wp-content/uploads/2017/04/DescriptorHeapAllocationsManager.png)

## CPUDescriptorHeap

![CPUDescriptorHeap](http://diligentgraphics.com/wp-content/uploads/2017/04/CPUDescriptorHeap.png)

## GPUDescriptorHeap

![GPUDescriptorHeap](http://diligentgraphics.com/wp-content/uploads/2017/04/GPUDescriptorHeap.png)

## Overview

![DescriptorHeaps-BigPicture1](http://diligentgraphics.com/wp-content/uploads/2017/04/DescriptorHeaps-BigPicture1-1024x557.png)

4 CPU-only descriptor heaps and 2 shader-visible descriptor heaps

Each thread contains one `DynamicSuballocationsManager`

## Usage Details

### Creating Resource Views

Example of creating a texture SRV:

1. Allocate a handle from the CPU-only descriptor heap
2. Use the D3D12 device to create an SRV on the handle
3. When releasing the SRV, consider frame synchronization (e.g., if three frames exist simultaneously, release after 3
   frames)

### Allocating Dynamic Descriptors

1. Allocate handles from the `DynamicSuballocationsManager` and create views on them
2. When releasing, consider frame synchronization (e.g., if three frames exist simultaneously, release after 3 frames)