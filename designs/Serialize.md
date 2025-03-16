# Serialization

The engine core uses [MySRefl](https://github.com/shimakaze09/MySRefl) for serialization. Users can also customize serialization methods for specific components.

## 1. Serialize

First, we support basic types:

- `float`, `double`
- `bool`
- `uint{8|16|32|64}_t`, `int{8|16|32|64}_t`
- `nullptr`

Special simple types include:

- Entity: Only records index (non-stable), used only for deserialization
- Asset: If a non-`nullptr` pointer, checks if it's an asset, and if so, records its GUID

Containers that will be supported:

- vector, array
- set, unordered_set
- tuple, pair
- map, unordered_map

Custom containers will be supported (through specialized macros)

For unsupported types, users can define custom functions

## 2. Deserialize

The world needs to register traits

The rest is implemented following the reverse process
