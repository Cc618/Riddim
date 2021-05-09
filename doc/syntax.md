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

### Macros
Macros are normal functions, only the syntax is different.
There is no parenthesis and every argument is passed on the same line.

```python
macro_keyword
macro_keyword arg1
macro_keyword arg1, arg2, etc
```

Here is the list of all macros :

| Keyword | Args | Description |
| ------- | ---- | ----------- |
| assert | The expression to test and optionally an error message | Prints to stdout all its args |
| print | Variadic | Prints to stdout all its args |
| rethrow | None | Rethrows the error caught within the catch block |
| throw | The error | Throws the error |
| typeof | The target | Returns the type of the target |

Macros cannot be defined by the user, they are always associated to a specific
keyword unavailable to the user.
That is, it is not possible to create a variable named print with the normal syntax.
Nevertheless, it is possible to surround a macro keyword with parenthesis to
make it an identifier :

```python
# Syntax error
print throw

# Sets the throw variable
(throw) = 42
print (throw)

# Prints Builtin<print>
print (print)

# In addition to macros keywords, it is possible to use other keywords :
(if) = 618
print (if)

# Or even operators
(+) = |a, b| a + b
print (+)(42, 618)
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

```python
# The iterator is the object that can be
# advanced with the @next slot
newtype MyIterator(n) {
    me.i = 1
    me.n = n
}

# Yields the next value or enditer
fn MyIterator@next() {
    # Terminate the iteration loop
    if me.i > me.n {
        return enditer
    }

    val = me.i
    me.i *= 2

    return val
}

# The main object, yields its iterator (MyIterator)
# in the @iter slot
newtype MyIterable(n) {
    me.n = n
}

fn MyIterable@iter() {
    # Works also with iter(1 ->= me.n)
    # to yield all numbers from 1 to n included
    return MyIterator(me.n)
}

iterable = MyIterable(64)

# Prints 1, 2, 4, 8, ..., 64
for i in iterable {
    print i
}

# Collects all values in a vector
print Vec(iterable)
```

This example shows how to create a custom iterable and iterator.
Note that you can create only an iterable and return a iterator
object of another iteratable using the iter builtin (see the
commented section of MyIterable@iter).

## Declarations
### Variables
We call variable declarations assignments.
An assignment is firstly an expression, that is, we can assign
multiple variables at once :
```python
a = b = 'I am a or b'
```

Riddim has also relative assignments :
```python
a += ' (I am a finally)'
```

The let keyword is used to make tuple assignments (variable destructuring).
It can be used in an assignment or in a for loop :
```python
# a = 1, b = 2
let a, b = [1, 2]

# a = 1, b = 2, c = 3
let a, (b, c) = [1, [2, 3]]

for let a, b in [[1, 2], [3, 4]] {
    print a, b
}
```

Note that it is possible to return a tuple from a return statement :
```python
return 1, 2, 3

# Same as
return [1, 2, 3]
```

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
block_lambda = |x, y| -> {
    print x + y
}

block_lambda(expression_lambda(1, 1), 2)
```

### Custom Types (classes)
In Riddim, classes are named types and instances objects.
It is possible to create custom types with custom methods and attributes.

#### Declaration and creation
To declare a new type, use the **newtype** keyword :
```python
newtype Obj
```

Now, Obj can be called to create a new object of the *Obj* type :
```python
myobject = Obj()
```

Any custom object has dynamic attributes.
That is, we can set attributes / methods anywhere in the code.
```python
myobject.name = 'Cc'
print myobject
# Displays Obj({name: Cc})
```

This type has no constructor (only a default one that does not take any argument).
To provide a custom constructor, declare after the newtype declaration :
```python
newtype Person(name, age) {
    me.name = name
    me.age = age
}
```

The variable *me* is like *this* or *self* in most object oriented languages,  it contains the object to instantiate.

Now, a person can be created like this :
```python
# Like in functions, you can use positional or keyword arguments
person = Person('Ada', 205)
person2 = Person(name: 'Cc', age: 618)
```

#### Methods
Like in Rust, methods can be declared anywhere in the source file.
As said in the previous section, **me** is equivalent to *this* or *self* in object oriented languages.

```python
fn Person.display() {
    print 'Person( name:', me.name, 'age:', me.age, ')'
}

fn Person.birthday() {
    me.age += 1
}
```

#### Special methods / operator overloading (slots)
It is possible to overload most object slots.
A slot is a special function that handle the behaviour of
and operator or other internal functions.

All slots begin with the **@** symbol.

```python
newtype Number(n) {
    me.n = n
}

fn Number@add(other) {
    return Number(me.n + other.n)
}

fn Number@neg() {
    return Number(-me.n)
}

fn Number@str() {
    return Str(me.n) + "!"
}

a = Number(314)
b = Number(304)

print a + b # 618!
print -a # -314!
# If @neg is overloaded, @sub can be omitted and is
# replaced by a + (-b)
print a - b # 10!
```

To have a list and a description of all slots, see [the object/Slots documentation](object.md).
Note that not all slots are allowed to be overloaded.

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

### Try catch
Try catch blocks are supported, they allow you to catch a specific
(or any) exception.
Here is the syntax :
```cpp
try {
    my_dangerous_code()
} catch ErrorType1 as error {
    print 'ErrorType1 :', error
} catch ErrorType2 as _ {
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

### Rethrow
You are allowed to rethrow the current error in a catch block with the rethrow keyword :
```cpp
try {
    throw 404
} catch Int as error {
    if error == 404 {
        rethrow
    }
}
```

### Assert
A useful macro is **assert**.
It throws an AssertError if the condition is not fulfilled.
In addition, an (optional) error message can be provided to explain
in depth why the error is thrown.

```python
# Without message
assert x >= 0

# With message
assert x >= 0, 'x must be non negative'
```

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

## Operator precedence
The precedence is similar to Python.
| Operator |
| - |
| = |
| or |
| and |
| not |
| == <= >= < > is in |
| \+ - |
| \* / % |
| Attribute reference |
| Indexing |
