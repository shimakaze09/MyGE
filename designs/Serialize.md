# Serialization

The engine core uses [MySRefl](https://github.com/shimakaze09/MySRefl) for serialization. Users can also customize
serialization methods for specific components.

## TODO

- [ ] Custom pointer operations

## 1. Serialize

First, support for basic types:

- `float`, `double`
- `bool`
- `uint{8|16|32|64}_t`, `int{8|16|32|64}_t`
- `nullptr`

Special simple types include:

- Entity: only records the index (non-stable), only used for deserialization
- asset: checks if a non-`nullptr` pointer is an asset, if so, records its GUID

Supported containers:

- Ordered fixed-length: `array`
- Ordered variable-length: `vector`, `list`, `forward_list`, `deque`, `set`, `unordered_set`, `multiset`,
  `unordered_multiset`
- Tuples: `tuple`, `pair`
- Associative: `map`, `unordered_map`, `multimap`, `unordered_multimap`

Support for custom containers (via specialization macros), such as:

- Ordered fixed-length: MyGM's `vec`, `val`, etc.

### TODO

- [x] For unsupported types, users can define custom functions
- [ ] reserve

## 2. Deserialize

First, iterate once to create an entity index mapping table (used for entity index remapping)

Implement according to the reverse process

### TODO

- [x] Users provide their own world (with registered cmpt traits, no entity)
- [x] Add multiple components at once
- [ ] reserve
- [x] Support for append mode (index mapping table includes version)