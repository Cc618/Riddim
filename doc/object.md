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
| Module | Gathers variables and code of a module with its functions etc |
| *Error | An exception (see [Errors](error.md) for all types) |
| Null | Describes a null reference |
| Bool | Boolean (true or false) |
| Int | An integer |
| Str | A string of characters |
| Vec | A collection that contiguously stores items and has a dynamic length |
| HashMap | A collection that serves as a mapping between keys and values, mappings are made given the hash of the key |
| AttrObject | An object that can contain custom attributes |

### Default methods
<!-- TODO : Abstract -->
| Name | Description | Expected Return Type | Example | Can be overloaded |
| ---- | ----------- | -------------------- | ------- | ----------------- |
| @add | + operator | | 2 + 2 | Yes |
| @call | Call operator (for functor like objects) | | f(2, a=22) | Yes |
| @cmp | Comparison, returns 0 on equality, < 0 if lesser or on inequality and > 0 if greater | Int | 1 <=> 2 returns -1 | Yes |
| @copy | Shallow copy / clone (by default it is the identity function) | | | No |
| @div | / operator | | 2 / 2 | Yes |
| @getattr | Get map attribute (not a read only attribute) | | a.foo which is a.@getattr('foo') | No |
| @getitem | Index / subscript getter | | a[42] which is a.@getitem(42) | Yes |
| @hash | Hash representation (by default, the address of the reference is returned) | Int | | Yes |
| @in | Whether an item is within a collection | Bool | 1 in [1, 2, 3] == true | Yes |
| @iter | Returns an iterator that iterates from the start of the object | Iterator-like (has @next method) | | Yes |
| @mod | % operator | | 2 % 2 | Yes |
| @mul | * operator | | 2 * 2 | Yes |
| @neg | - (unary, negation) operator | | -2 | Yes |
| @next | Returns either the next item of an iterator or enditer | | | Yes |
| @setattr | Set map attribute (not a read only attribute) | | a.foo = 42 which is a.@setattr('foo', 42) | No |
| @setitem | Index / subscript setter | Null | a[42] = 2 which is a.@setitem(42, 2) | Yes |
| @str | String representation (by default it returns 'TypeName()' | Str | @str(Int) which returns 'Type(Int)' | Yes |
| @traverse_objects | Traverses all objects owned by this object (used by the garbage collector, internal function) | | | No |
| @sub | - operator | | 2 - 2 | Yes |
