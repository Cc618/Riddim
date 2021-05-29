# Doc Strings
This page describes the format of a doc string.
Please, see [the syntax documentation](syntax.md) for an introduction.

## Doc string syntax
It is possible to document your functions / types using doc strings :
```python
# Functions
doc'My fib function'
fn fib() {
}

# Types
doc"My type"
newtype MyType

# Methods
@doc'
You can use raw strings too
@'
fn MyType.fac() {
}
```

To display this documentation, use the **doc** builtin :
```python
print doc(fib)
# prints 'My fib function'
```

**Tip** : The !doc attribute variable can be set on most objects,
it contains the doc string if @doc is not overriden.

## Documentation syntax
The format is inspired by Markdown.

### Basic syntax
```markdown
Method small description.

- Arg1, Arg1Type (if necessary) : Arg1 description
- [OptionalArg2], Arg2Type : Arg2 description
- return, ReturnType : Return description
```

### Complete syntax
The complete format is described below.

*Note that the generator is not implemented for version v0.1, this section is a draft for the future documentation generator.*

#### Text
```markdown
This is a small description.
*italic word* **bold word** ***italic and bold word***.
<www.url>

Two newlines = new paragraph

\> : escaped character (>)
```

#### Headers
```markdown
# Module
## Object
### Attribute
#### Custom section
```

#### Lists
```markdown
+ Item 1
+ Item 2
+ Item 3
```

#### Arguments
```markdown
- fun, Functor : The function to test
- arg, Type : Description
- [arg], Type : Optional arg
- [args...], Type : Description
- return, Type : Description
```

#### Annotations
```markdown
* Note : See also [path.to.mod]
* Note : Description
* Warning : Description
* Error, AssertionError : If returns false
* Error : 42 might be thrown
* Example : `
test.assert_true(false)
`
```

#### Code Listings
```markdown
> inline_code()

`
line1()
line2()
`
```

## Documentation Generator (autodoc)
A documentation generator is provided with Riddim.
The builtin function autodoc generates documentation for all provided modules :
```python
modules = {
    'Builtins': builtins,
    'Math': math,
}

notes = {
    'Version': 'v0.0.1a',
}

autodoc('tests/autodoc', modules, notes: notes, lowercase_filenames: true)
```

This example generates these files :
- tests/autodoc/builtins.md : Builtins module documentation
- tests/autodoc/math.md : Math module documentation
- tests/autodoc/README.md : Summary with notes
