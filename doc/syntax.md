# Syntax
## General
### Comment
Comments are line comments starting with # :
```py
# I am a comment
```

### Statement ending
All statements are terminated by a line feed.
```py
a = 10
```

You can escape line feeds using a backslash (\\) :
```py
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
```py
a = 'Hello'
b = "World"
```

Some characters can be escaped :
```py
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
```
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
```
# Error if a is not a Bool
if a {
    # ...
}
```

### While
While is the most simple loop :
```
while condition {
    # ...
}
```
