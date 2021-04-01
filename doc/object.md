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
| *Error | An exception (see [Errors](error.md) for all types) |
| Int | An integer |
| Str | A string of characters |
| HashMap | A collection that serves as a mapping between keys and values, mappings are made given the hash of the key |

### Default methods
<!-- TODO : Abstract -->
<!-- TODO : Update if str changed etc... -->
| Name | Description | Expected Return Type | Example |
| ---- | ----------- | -------------------- | ------- |
| @getattr | Get map attribute (not a read only attribute) | | a.foo which is a.@getattr('foo') |
| @getitem | Index / subscript getter | | a[42] which is a.@getitem(42) |
| @hash | Hash respresentation | Int | |
| @setattr | Set map attribute (not a read only attribute) | | a.foo = 42 which is a.@setattr('foo', 42) |
| @setitem | Index / subscript setter | Null | a[42] = 2 which is a.@setitem(42, 2) |
| @str | String representation | Str | @str(Int) which returns 'Type(Int)' |
