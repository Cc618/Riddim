# Syntax
## General
### Comment
Comments are line comments starting with # :
```python
# I am a comment
```

### Statement ending
All statements are terminated by a line feed.
```python
a = 10
```

You can escape line feeds using a backslash (\\) :
```python
a = 5 + \
    5
```

## Literals
### Integer
Integers can be written in multiple ways :
```
# Decimal
42
# Hexadecimal
0xFF
# Binary
0b101
# We can have ' or _ to separate digits
1'234
1_234
0xFF_FF
```

### String
Strings can be declared by single or double quotes :
```python
a = 'Hello'
b = "World"
```

Some characters can be escaped :
```python
a = 'Hello\tworld.\nI\'m coding in Riddim !'
```

Here is the list of all escapes :

| Escape | Output |
| ------ | ------ |
| \\' | Single quote (') |
| \\" | Double quote (") |
| \\\\ | Back slash (\\) |
| \\n | Line feed |
| \\t | Tab |

## Primary expressions
### Indexing
Indexing is done with brackets like in many languages.
```python
a[key1] = b[key2]
```

The key can be any object (Int for Vec, Object for HashMap...).

### Calls
Calls have a Dart-like syntax, keyword valued arguments (kwargs)
have the colon symbol to separate keywords and values.
```python
# Only positional arguments
fun(a, b, c)

# Positional (first) and then keyword (last) arguments
# Kwargs don't have a specific position
# fun : fn(a, b, c)
fun(a, c: 1, b: 7)
```

Moreover in Riddim, everything is an object.
That is, we don't really have functions, we have functor objects.
```python
impostor = (print)
impostor('I am not print')
```

## Control Flow
### If / Else / Elif
Here is the if syntax :
```python
if condition1 {
    # Do something
} elif condition2 {
    # Do other thing
} else {
    # Do this if every condition is false
}
```

All conditions must be booleans, it is not possible to check whether
a variable is null like in this example :
```python
# Error if a is not a Bool
if a {
    # ...
}
```

### While
While is the most simple loop :
```python
while condition {
    # ...
}
```

### For
A for loop iterates within an iterable.
An iterable can be a collection like a Vec or
an object like Range
```rust
vec = [1, 2, 3]
for item in vec {
    print item
}
```

```python
>>> 1
>>> 2
>>> 3
```

#### Ranges
A range is an iterable that creates an iterator yielding a sequence of
number.
It can be created with the arrow syntax :
```python
# 0 to 3 excluded (0, 1, 2)
0 -> 3

# 0 to 8 excluded with step 2 (0, 2, 4, 6)
0 -> 8 .. 2

# 0 to 2 included (0, 1, 2)
0 ->= 2

# 0 to 4 included with step 2 (0, 2, 4)
0 ->= 4 .. 2

# Use ranges like this
for i in 0 -> 4 {
    print i
}
```

```python
>>> 0
>>> 1
>>> 2
>>> 3
```

#### Iterables and Iterators
An iterable is an object that overrides the @iter method.
This function returns a new Iterator located at the beginning
of the iteration.

For instance, 0 -> 2 is an Iterable that will return an Iterator
when @iter is called.

An iterator is an object that overrides the @next function.
This method returns the next item or enditer.
enditer is a special object that describes the end of the iterator.

For instance, when iterating through 0 -> 2, the @next method yields :
- 0
- 1
- enditer

For loops call the @iter method of the object to iterate through (like a range).
And then, @next is called every iteration until it returns enditer.

## Declarations
### Functions
#### Statement
Function declarations are like assignment statements.
They store a functor object interpreting the body of the function in a variable.
```python
# foo is now a function
fn foo(s) {
    print 'foo() :', s
}

# The attribute foo of the obj variable is set
fn obj.foo(s) {
    print 'obj.foo() :', s
}
```

#### Arguments
Arguments can have a default value :
```python
# a and b can be omitted in a call
# Note that only b can be set using kwargs (see calls section)
fn fun(a: 42, b: 618) {
    print a, b
}
```

#### Return value
Like Python, the return keyword returns a value from a function :
```rust
fn fun(a, b) {
    return a + b
}
```

Note that the return keyword can be use to exit from a module :
```python
# main.rid
a = 1
return

# Unreachable
b = 2
```

#### Lambas
Lambdas (or closures) have a syntax similar to Rust's closures :
```rust
expression_lambda = |x, y| x + y
block_lambda = |x, y| {
    print x + y
}

block_lambda(expression_lambda(1, 1), 2)
```

## Error handling
### Throw
To raise an error, the throw keyword can be used.
Theorically, any object can be thrown, even a non error object.
The syntax is simple, throw is a macro and takes one expression :
```python
if x <= 0 {
    throw ArgumentError('x must be positive !')
}
```

<!-- TODO : Assert -->

### Try catch
Try catch blocks are supported, they allow you to catch a specific
(or any) exception.
Here is the syntax :
```cpp
try {
    my_dangerous_code()
} catch ErrorType1 as error {
    print 'ErrorType1 :', error
} catch ErrorType2 {
    print 'ErrorType2 caught'
} catch error {
    print 'Unknown error type :', error
}
```

In this example, if my_dangerous_code leaves an error of type ErrorType1,
the first catch block is executed and error is set to the current error.
Otherwise the second block is executed if ErrorType2 matches the type of
the error.
Finally, the last block is executed if there is another error.
Note that only one catch block can be used.
You can throw in a catch block, it is useful to rethrow the current error.

## Imports (use keyword)
The use keyword is used to import modules.
A module is a Riddim file (with the .rid extension).
```python
# Loads ./dir/lib.rid into the lib variable
use dir.lib

# Loads ./lib2.rid into the anotherlib variable
use lib2 as anotherlib

# Merges all variables of lib3 within this module
use lib3.*
```

<!-- TODO : Print / builtins -->

## Operator precedence
The precedence is similar to Python.
- =
- or
- and
- not
- == <= >= < > is in
- \+ -
- \* / %
- Attribute reference
- Indexing
