# Objects
<!-- TODO : Abstract -->
<!-- TODO : C++ init -->

## Types
### Builtins
| Type | Description |
| ---- | ----------- |
| Object | Does nothing particular in the Riddim side, it is the base class for every objects in C++ |
| Type | Describes the type of any object, every type is unique |
| Program | Contains every global objects and informations such as error state, it is the root used in the garbage collector algorithm |
| Int | An integer |
| Str | A string of characters |
| HashMap | A collection that serves as a mapping between keys and values, mappings are made given the hash of the key |

### Default methods
<!-- TODO : Abstract -->
<!-- TODO : Update if str changed etc... -->
| Name | Description | Expected Return Type | Example |
| ---- | ----------- | -------------------- | ------- |
| @index | Indexing |  | a[42] which is a.@index(42) | |
| @str | String representation | Str | @str(Int) which returns 'Type(Int)' |
| @hash | Hash respresentation | Int | |
