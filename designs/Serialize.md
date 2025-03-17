# Serialization

The engine core uses [MySRefl](https://github.com/shimakaze09/MySRefl) for serialization. Users can also customize
serialization methods.

## 1. Serialize

Basic types are supported first:

- `float`, `double`
- `bool`
- `uint{8|16|32|64}_t`, `int{8|16|32|64}_t`
- `nullptr`

Special simple types include:

- Entity: only records the index (non-stable), only used for deserialization
- asset: if a non-`nullptr` pointer is detected as an asset, its GUID is recorded

Supported containers:

- Ordered fixed-length: `array`
- Ordered variable-length: `vector`, `list`, `forward_list`, `deque`, `set`, `unordered_set`, `multiset`,
  `unordered_multiset`
- Tuples: `tuple`, `pair`
- Associative: `map`, `unordered_map`, `multimap`, `unordered_multimap`

Custom containers are supported (through macro specialization), such as:

- Ordered fixed-length: MyGM's `vec`, `val`, etc.

Types registered with TypeInfo are supported

Custom type handling is supported

## 2. Deserialize

First, iterate through once to calculate the entity index mapping table (used for entity index remapping)

Implementation follows the reverse process