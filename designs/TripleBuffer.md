# Triple Buffer

The buffers mentioned here are dynamic variable buffers, such as per object constant buffer, per material constant
buffer, etc.

Since CPU and GPU work asynchronously, at least double buffering is needed to achieve parallelism between CPU and GPU.

> After the CPU submits rendering instructions, it can proceed to the next frame without waiting for the GPU to complete
> the rendering instructions. However, when the CPU needs to fill new frame data (various constant buffers) to the GPU, if
> the GPU is still reading these buffers, a conflict occurs, and it must wait for the GPU to complete execution, then the
> GPU waits for the CPU to update the data before ending the wait. Double buffering can solve this problem (assuming the
> GPU is the bottleneck or CPU and GPU have similar processing times).

In scenarios where CPU and GPU take about the same time per frame, with only double buffering, delays may occur in CPU
and GPU operations [^triple_buffer] due to the latency in command submission from CPU to GPU and GPU notifications back
to CPU. Triple buffering can avoid this problem (when either CPU or GPU is a clear bottleneck, this latency has no
impact).

![image: ../Art/ResourceManagement_TripleBuffering_2x.png](https://developer.apple.com/library/archive/documentation/3DDrawing/Conceptual/MTLBestPracticesGuide/Art/ResourceManagement_TripleBuffering_2x.png)

> In the image, "Commit Frame n" and "Frame n callback" represent the latency caused by command transmission

In summary, double buffering resolves data conflicts, while triple buffering addresses command transmission latency.

# References

[^triple_buffer]: [Metal : Triple Buffer](https://developer.apple.com/library/archive/documentation/3DDrawing/Conceptual/MTLBestPracticesGuide/TripleBuffering.html)