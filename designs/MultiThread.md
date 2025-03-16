# Multithreading

## Pipeline

CPU tasks are as follows:

1. world update (**parallel**)
2. world -> context
3. wait
4. frame graph
    1. register resource nodes
    2. register pass nodes
    3. compile frame graph
    4. **parallelly** execute frame graph (avoid conflict for some operations)
5. sequentially commit command lists to the command queue

If these steps are executed sequentially, some CPU resources would be wasted (not utilizing parallelism)

Consider using **multithreaded pipeline mode**, dividing into two pipeline stages:

- 1-2: update
- 3-5: draw

> Both stages have internal multithreading, so generally all cores run at full capacity
>
> Additionally, update is typically longer than draw, so according to the barrel effect, there's no need to further divide draw
>
> If needed (draw is indeed longer than update, and CPU is the bottleneck), draw can be further divided
>
> Too many pipeline stages will cause latency issues

## Triple Buffering

The buffering here refers to dynamic variable buffers, such as per object constant buffer, per material constant buffer, etc.

Since CPU and GPU operate asynchronously, double buffering is required at minimum to enable parallelism between CPU and GPU

> After CPU submits render instructions, it can enter the next frame without waiting for GPU to complete rendering instructions. However, when CPU needs to fill new frame data (various constant buffers) to GPU, if GPU is still reading these buffers, a conflict occurs, requiring waiting for GPU execution to complete before updating data. Double buffering solves this problem (assuming GPU is the bottleneck or CPU and GPU take similar time).

Considering situations where CPU and GPU take similar time per frame, with only double buffering, due to latency in command submission from CPU to GPU and GPU notifying CPU of render completion, stalls may occur in both CPU and GPU [^triple_buffer]. Triple buffering can avoid this problem (when CPU or GPU is clearly the bottleneck, this latency has no impact)

![image: ../Art/ResourceManagement_TripleBuffering_2x.png](https://developer.apple.com/library/archive/documentation/3DDrawing/Conceptual/MTLBestPracticesGuide/Art/ResourceManagement_TripleBuffering_2x.png)

> In the diagram, "Commit Frame n" and "Frame n callback" represent the command transfer latency

In summary, double buffering solves data conflicts, while triple buffering solves command transfer latency

# References

[^triple_buffer]: https://developer.apple.com/library/archive/documentation/3DDrawing/Conceptual/MTLBestPracticesGuide/TripleBuffering.html