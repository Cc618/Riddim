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
    print(item)
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
    print(i)
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

<!-- TODO : Not an h2 -->
## Indexing
Indexing is done with brackets like in many languages.
```
a[key1] = b[key2]
```

<!-- TODO : Print -->

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
