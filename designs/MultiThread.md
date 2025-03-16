# Multi-Threading

CPU work is as follows:

1. World update (**parallel**)
2. Wait and world -> GPU (flush)
3. Frame graph
    1. Register resource nodes
    2. Register pass nodes
    3. Compile frame graph
    4. **Parallelly** execute frame graph (avoid conflict for some operations)
    5. Sequentially commit command lists to the command queue

Steps 1 and 3 are internally parallel, and the world cannot be modified during step 2.

- Level 1: Directly execute each step serially, and 3.4 is executed serially

- Level 2: 3.4 in parallel
- Level 3: Pipeline parallel mode for steps 1-2 and 3

- Level 4: To implement pipeline parallel mode for steps 1-2 as well, data uploads are divided into 3 tiers:
    - Tier 1: Upload large dynamic data (meshes, textures, etc.), copy small data (pass, material, object constants)
    - Tier 2: Mark static data that needs uploading as locked (not allowed to delete), allowing step 1 to run, then
      upload this data and gradually unlock
    - Tier 3: Upload small data

Level 2 will definitely be implemented.

Level 3 can be implemented later.

Level 4 will have a significant impact on the entire framework.