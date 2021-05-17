# Doc Strings
This page describes the format of a doc string.
Please, see [the syntax documentation](syntax.md) for an introduction.

## Syntax
The format is inspired by Markdown.

The basic format for a method is :
```markdown
Method small description.

- Arg1, Arg1Type (if necessary) : Arg1 description
- [OptionalArg2, Arg2Type] : Arg2 description
- return, ReturnType : Return description
```

The complete format is described below.
*Note that the generator is not implemented as of v0.1, this section is a draft for the future documentation generator.*
```markdown
1. Text :
This is a small description.
*italic word* **bold word** ***italic and bold word**.
<www.url>

Two newlines = new paragraph

1. Headers :
# Module
## Object
### Attribute
#### Custom section

1. Escapes :
\> : escaped character (>)

1. Lists :
+ Item 1
+ Item 2
+ Item 3

1. Arguments :
- fun, Functor : The function to test
- arg, Type : Description
- [arg], Type : Optional arg
- [args...], Type : Description
- return, Type : Description

1. Annotations :
* Note : See also [path.to.mod]
* Note : Description
* Warning : Description
* Error, AssertionError : If returns false
* Error : 42 might be thrown
* Example : `
test.assert_true(false)
`
1. Code sections :
> inline_code()

`
line1()
line2()
`
```
