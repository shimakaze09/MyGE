# PSO Management

First, let's clarify the concepts and relationships between HLSL files, shaders, materials, passes, root signatures, and PSOs in this engine:

- HLSL file: Must include VS and PS
- Shader: HLSL file + VS & PS entry points + target + name
- Material: Shader + properties (specific textures, specific float4 values, etc.)
- Pass: A series of rendering operations (with the same Render Target), such as geometry pass. Internally supports multiple shaders, each shader supports multiple materials, and each material supports multiple objects (each object is a submesh)
- Root Signature: Root parameters (input format for shaders). Generally has a 1-to-1 relationship, though multiple shaders can share 1 root signature
- PSO: Rendering configuration. Within a pass, determined by the object (different mesh layouts can correspond to different PSOs)

First, we generate all mesh layouts supported by the engine (roughly within 100, currently 16)

Then, based on requirements, we dynamically/preemptively generate PSOs corresponding to different mesh layouts
