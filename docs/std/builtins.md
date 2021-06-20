[Summary](README.md)

# builtins
Auto imported module.
Contains built-in variables.

## builtins.Deque
## Deque
Double ended queue data type.
A queue supporting front / back operations

### Deque.add
Pushes back an item

> Deque.add(item)

### Deque.add_front
Pushes front an item

> Deque.add_front(item)

### Deque.bin_search
Returns the index of item in a sorted collection using
the binary search algorithm.
Returns -1 if not found.

- item : Item to find
- return : The index of the item (or -1 if not found)

> Deque.bin_search(item)

### Deque.empty
Returns whether it is empty
- return, Bool

> Deque.empty()

### Deque.filter
Returns a collection with all values that
matches the predicate f

- f : Predicate
- [init] : The collection to use

> Deque.filter(f, [init])

### Deque.for_each
Applies f to every item of the collection

- f : Functor

> Deque.for_each(f)

### Deque.index
Returns the index of the first occurence of item.
Returns -1 if not found.

- item : Item to find
- return : The index of the item (or -1 if not found)

> Deque.index(item)

### Deque.last_index
Returns the index of the last occurence of item.
Returns -1 if not found.

- item : Item to find
- return : The index of the item (or -1 if not found)

> Deque.last_index(item)

### Deque.map
Returns a collection with all values mapped
by f

- f : Functor
- [init] : The collection to use

> Deque.map(f, [init])

### Deque.not_empty
Returns whether it is not empty

- return, Bool

> Deque.not_empty()

### Deque.pop
Removes an item at a target index (by default, the last item is popped out)

- [index : -1], Int / Range : Where to pop
- return : The popped item(s)

> Deque.pop([index])

### Deque.pop_front
Removes the first item and returns it.
It is an alias for pop(0)

- return : Popped item

> Deque.pop_front()

### Deque.reduce
Returns the cumulative application of f
to all elements starting by (init, first element)

- f : Functor of 2 parameters
- [init] : The collection to use

> Deque.reduce(f, [init])

### Deque.sort
Sorts the collection inplace

- [cmp] : Custom compare, returns a <=> b, by default a@cmp is used
- return : Returns col

> Deque.sort([cmp])

### Deque.sum
Returns the sum of all items.
Returns empty_result if the collection is empty

- empty_result : Result to return if the collection is empty
- return : The sum of all items

> Deque.sum([empty_result])

## builtins.File
## File
A file stream.
Can be material or virtual

- path : Path (relative or absolute) to the target file
- [mode, Str] : Opening mode, string describing how to open the
    file, see details below. 'r' by default

* Opening modes :
+ r : Read only
+ w : Write only
+ rw : Read and write
+ rb : Read only (binary mode)
+ wb : Write only (binary mode)
+ rwb : Read and write (binary mode)

### File.close
Closes the file

> File.close()

### File.read
Reads the content of the file

- return, Str (text mode) or Vec{Int} (binary mode) : Content

> File.read()

### File.read_char
Reads the next character (or byte) of the file

- return, Str (text mode) or Int (binary mode) or null : Character / byte, null if end of file

> File.read_char()

### File.read_line
Reads the next line of the file

- return, Str or Null : Line or null if end of file
* Mode must be textual

> File.read_line()

### File.stderr
Standard error output stream

### close
Closes the file

> File.close()

### read
Reads the content of the file

- return, Str (text mode) or Vec{Int} (binary mode) : Content

> File.read()

### read_char
Reads the next character (or byte) of the file

- return, Str (text mode) or Int (binary mode) or null : Character / byte, null if end of file

> File.read_char()

### read_line
Reads the next line of the file

- return, Str or Null : Line or null if end of file
* Mode must be textual

> File.read_line()

### stdin
### close
Closes the file

> File.close()

### read
Reads the content of the file

- return, Str (text mode) or Vec{Int} (binary mode) : Content

> File.read()

### read_char
Reads the next character (or byte) of the file

- return, Str (text mode) or Int (binary mode) or null : Character / byte, null if end of file

> File.read_char()

### read_line
Reads the next line of the file

- return, Str or Null : Line or null if end of file
* Mode must be textual

> File.read_line()

### write
Writes the textual / binary content to the file

- s, Str (text mode) or Iterable of Int (binary mode) : Content

> File.write()

### stdout
### close
Closes the file

> File.close()

### read
Reads the content of the file

- return, Str (text mode) or Vec{Int} (binary mode) : Content

> File.read()

### read_char
Reads the next character (or byte) of the file

- return, Str (text mode) or Int (binary mode) or null : Character / byte, null if end of file

> File.read_char()

### read_line
Reads the next line of the file

- return, Str or Null : Line or null if end of file
* Mode must be textual

> File.read_line()

### stdin
### close
Closes the file

> File.close()

### read
Reads the content of the file

- return, Str (text mode) or Vec{Int} (binary mode) : Content

> File.read()

### read_char
Reads the next character (or byte) of the file

- return, Str (text mode) or Int (binary mode) or null : Character / byte, null if end of file

> File.read_char()

### read_line
Reads the next line of the file

- return, Str or Null : Line or null if end of file
* Mode must be textual

> File.read_line()

### write
Writes the textual / binary content to the file

- s, Str (text mode) or Iterable of Int (binary mode) : Content

> File.write()

### write
Writes the textual / binary content to the file

- s, Str (text mode) or Iterable of Int (binary mode) : Content

> File.write()

### write
Writes the textual / binary content to the file

- s, Str (text mode) or Iterable of Int (binary mode) : Content

> File.write()

### File.stdin
Standard input stream

### close
Closes the file

> File.close()

### read
Reads the content of the file

- return, Str (text mode) or Vec{Int} (binary mode) : Content

> File.read()

### read_char
Reads the next character (or byte) of the file

- return, Str (text mode) or Int (binary mode) or null : Character / byte, null if end of file

> File.read_char()

### read_line
Reads the next line of the file

- return, Str or Null : Line or null if end of file
* Mode must be textual

> File.read_line()

### write
Writes the textual / binary content to the file

- s, Str (text mode) or Iterable of Int (binary mode) : Content

> File.write()

### File.stdout
Standard output stream

### close
Closes the file

> File.close()

### read
Reads the content of the file

- return, Str (text mode) or Vec{Int} (binary mode) : Content

> File.read()

### read_char
Reads the next character (or byte) of the file

- return, Str (text mode) or Int (binary mode) or null : Character / byte, null if end of file

> File.read_char()

### read_line
Reads the next line of the file

- return, Str or Null : Line or null if end of file
* Mode must be textual

> File.read_line()

### stdin
### close
Closes the file

> File.close()

### read
Reads the content of the file

- return, Str (text mode) or Vec{Int} (binary mode) : Content

> File.read()

### read_char
Reads the next character (or byte) of the file

- return, Str (text mode) or Int (binary mode) or null : Character / byte, null if end of file

> File.read_char()

### read_line
Reads the next line of the file

- return, Str or Null : Line or null if end of file
* Mode must be textual

> File.read_line()

### write
Writes the textual / binary content to the file

- s, Str (text mode) or Iterable of Int (binary mode) : Content

> File.write()

### write
Writes the textual / binary content to the file

- s, Str (text mode) or Iterable of Int (binary mode) : Content

> File.write()

### File.write
Writes the textual / binary content to the file

- s, Str (text mode) or Iterable of Int (binary mode) : Content

> File.write(s)

## builtins.HashMap
## HashMap
HashMap data type.
Creates a mapping between key / value pairs.
This is the object created with the {k: v} syntax.

### HashMap.empty
Returns whether it is empty
- return, Bool

> HashMap.empty()

### HashMap.filter
Returns a collection with all values that
matches the predicate f

- f : Predicate
- [init] : The collection to use

> HashMap.filter(f, [init])

### HashMap.for_each
Applies f to every item of the collection

- f : Functor

> HashMap.for_each(f)

### HashMap.index
Returns the index of the first occurence of item.
Returns -1 if not found.

- item : Item to find
- return : The index of the item (or -1 if not found)

> HashMap.index(item)

### HashMap.last_index
Returns the index of the last occurence of item.
Returns -1 if not found.

- item : Item to find
- return : The index of the item (or -1 if not found)

> HashMap.last_index(item)

### HashMap.map
Returns a collection with all values mapped
by f

- f : Functor
- [init] : The collection to use

> HashMap.map(f, [init])

### HashMap.not_empty
Returns whether it is not empty

- return, Bool

> HashMap.not_empty()

### HashMap.pop
Removes a mapping by key

- key : Key associated to mapping to pop
- return : The popped value

> HashMap.pop(key)

### HashMap.reduce
Returns the cumulative application of f
to all elements starting by (init, first element)

- f : Functor of 2 parameters
- [init] : The collection to use

> HashMap.reduce(f, [init])

### HashMap.sum
Returns the sum of all items.
Returns empty_result if the collection is empty

- empty_result : Result to return if the collection is empty
- return : The sum of all items

> HashMap.sum([empty_result])

## builtins.HashSet
## HashSet
HashSet data type.
Set with O(1) time to insert an item (best case).

### HashSet.add
Inserts an item

> HashSet.add(item)

### HashSet.empty
Returns whether it is empty
- return, Bool

> HashSet.empty()

### HashSet.filter
Returns a collection with all values that
matches the predicate f

- f : Predicate
- [init] : The collection to use

> HashSet.filter(f, [init])

### HashSet.for_each
Applies f to every item of the collection

- f : Functor

> HashSet.for_each(f)

### HashSet.index
Returns the index of the first occurence of item.
Returns -1 if not found.

- item : Item to find
- return : The index of the item (or -1 if not found)

> HashSet.index(item)

### HashSet.last_index
Returns the index of the last occurence of item.
Returns -1 if not found.

- item : Item to find
- return : The index of the item (or -1 if not found)

> HashSet.last_index(item)

### HashSet.map
Returns a collection with all values mapped
by f

- f : Functor
- [init] : The collection to use

> HashSet.map(f, [init])

### HashSet.not_empty
Returns whether it is not empty

- return, Bool

> HashSet.not_empty()

### HashSet.pop
Removes an item

- item : Which item to pop
- return : The popped item(s)

> HashSet.pop(item)

### HashSet.reduce
Returns the cumulative application of f
to all elements starting by (init, first element)

- f : Functor of 2 parameters
- [init] : The collection to use

> HashSet.reduce(f, [init])

### HashSet.sum
Returns the sum of all items.
Returns empty_result if the collection is empty

- empty_result : Result to return if the collection is empty
- return : The sum of all items

> HashSet.sum([empty_result])

## builtins.SegTree
## SegTree
Segment Tree, used to compute range queries
in O(log N) time

### SegTree.bin_search
Returns the index of item in a sorted collection using
the binary search algorithm.
Returns -1 if not found.

- item : Item to find
- return : The index of the item (or -1 if not found)

> SegTree.bin_search(item)

### SegTree.empty
Returns whether it is empty
- return, Bool

> SegTree.empty()

### SegTree.filter
Returns a collection with all values that
matches the predicate f

- f : Predicate
- [init] : The collection to use

> SegTree.filter(f, [init])

### SegTree.for_each
Applies f to every item of the collection

- f : Functor

> SegTree.for_each(f)

### SegTree.index
Returns the index of the first occurence of item.
Returns -1 if not found.

- item : Item to find
- return : The index of the item (or -1 if not found)

> SegTree.index(item)

### SegTree.last_index
Returns the index of the last occurence of item.
Returns -1 if not found.

- item : Item to find
- return : The index of the item (or -1 if not found)

> SegTree.last_index(item)

### SegTree.map
Returns a collection with all values mapped
by f

- f : Functor
- [init] : The collection to use

> SegTree.map(f, [init])

### SegTree.not_empty
Returns whether it is not empty

- return, Bool

> SegTree.not_empty()

### SegTree.query
Queries the segment tree from start (included) to end (excluded)

- start, Int : First index (included)
- end, Int : Last index (excluded)
- [init_val, Int] : The default value when the range is empty (initializes the query)
- return : The application of the functor on this range

* Note : It is equivalent to functor(data[start -> end]) but with log N time complexity

> SegTree.query(start, end, [init_val : 0])

### SegTree.reduce
Returns the cumulative application of f
to all elements starting by (init, first element)

- f : Functor of 2 parameters
- [init] : The collection to use

> SegTree.reduce(f, [init])

### SegTree.sort
Sorts the collection inplace

- [cmp] : Custom compare, returns a <=> b, by default a@cmp is used
- return : Returns col

> SegTree.sort([cmp])

### SegTree.sum
Returns the sum of all items.
Returns empty_result if the collection is empty

- empty_result : Result to return if the collection is empty
- return : The sum of all items

> SegTree.sum([empty_result])

## builtins.Str
## Str
String data type

### Str.add
Appends the other string

- s, Str : String to append

> Str.add(s)

### Str.bin_search
Returns the index of item in a sorted collection using
the binary search algorithm.
Returns -1 if not found.

- item : Item to find
- return : The index of the item (or -1 if not found)

> Str.bin_search(item)

### Str.chr
Returns the character associated to the code

> Str.chr(code)

### Str.empty
Returns whether it is empty
- return, Bool

> Str.empty()

### Str.erase
Removes all occurences of target

- target : Pattern to remove

* Note : equivalent to `replace(target, '')`

> Str.erase(target)

### Str.filter
Returns a collection with all values that
matches the predicate f

- f : Predicate
- [init] : The collection to use

> Str.filter(f, [init])

### Str.for_each
Applies f to every item of the collection

- f : Functor

> Str.for_each(f)

### Str.index
Returns the index of the first occurence of item.
Returns -1 if not found.

- item : Item to find
- return : The index of the item (or -1 if not found)

> Str.index(item)

### Str.join
Concatenates every item of the iterable into a string, the current
string separates every item.

- iterable : Iterable of elements containing a string representation
- return, Str : Concatenation of all items with 'me' as separator

> Str.join(iterable)

### Str.last_index
Returns the index of the last occurence of item.
Returns -1 if not found.

- item : Item to find
- return : The index of the item (or -1 if not found)

> Str.last_index(item)

### Str.lower
Transforms the string in lower case characters (inplace)

> Str.lower()

### Str.map
Returns a collection with all values mapped
by f

- f : Functor
- [init] : The collection to use

> Str.map(f, [init])

### Str.not_empty
Returns whether it is not empty

- return, Bool

> Str.not_empty()

### Str.ord
Returns the code associated to the character

> Str.ord(char)

### Str.reduce
Returns the cumulative application of f
to all elements starting by (init, first element)

- f : Functor of 2 parameters
- [init] : The collection to use

> Str.reduce(f, [init])

### Str.replace
Replaces all occurences of old by new

- old : What to search for
- new : What to replace

> Str.replace(old, new)

### Str.sort
Sorts the collection inplace

- [cmp] : Custom compare, returns a <=> b, by default a@cmp is used
- return : Returns col

> Str.sort([cmp])

### Str.split
Splits the string into substrings delimited by
delim.

- [delim, Str] : Every delimiters (' \n') will split
    spaces and new lines
- return, Vec{Str} : Substrings

> Str.split([delim])

### Str.sum
Returns the sum of all items.
Returns empty_result if the collection is empty

- empty_result : Result to return if the collection is empty
- return : The sum of all items

> Str.sum([empty_result])

### Str.upper
Transforms the string in upper case characters (inplace)

> Str.upper()

## builtins.TreeMap
## TreeMap
TreeMap data type.
Creates a mapping between key / value pairs.
Keys must be comparable since they are ordered (a BST is used).

### TreeMap.empty
Returns whether it is empty
- return, Bool

> TreeMap.empty()

### TreeMap.filter
Returns a collection with all values that
matches the predicate f

- f : Predicate
- [init] : The collection to use

> TreeMap.filter(f, [init])

### TreeMap.for_each
Applies f to every item of the collection

- f : Functor

> TreeMap.for_each(f)

### TreeMap.index
Returns the index of the first occurence of item.
Returns -1 if not found.

- item : Item to find
- return : The index of the item (or -1 if not found)

> TreeMap.index(item)

### TreeMap.last_index
Returns the index of the last occurence of item.
Returns -1 if not found.

- item : Item to find
- return : The index of the item (or -1 if not found)

> TreeMap.last_index(item)

### TreeMap.map
Returns a collection with all values mapped
by f

- f : Functor
- [init] : The collection to use

> TreeMap.map(f, [init])

### TreeMap.not_empty
Returns whether it is not empty

- return, Bool

> TreeMap.not_empty()

### TreeMap.pop
Removes a mapping by key

- key : Key associated to mapping to pop
- return : The popped value

> TreeMap.pop(key)

### TreeMap.reduce
Returns the cumulative application of f
to all elements starting by (init, first element)

- f : Functor of 2 parameters
- [init] : The collection to use

> TreeMap.reduce(f, [init])

### TreeMap.sum
Returns the sum of all items.
Returns empty_result if the collection is empty

- empty_result : Result to return if the collection is empty
- return : The sum of all items

> TreeMap.sum([empty_result])

## builtins.TreeSet
## TreeSet
TreeSet data type.
Set with O(log N) insert time, all values are ordered (and must be comparable).

### TreeSet.add
Inserts an item

> TreeSet.add(item)

### TreeSet.empty
Returns whether it is empty
- return, Bool

> TreeSet.empty()

### TreeSet.filter
Returns a collection with all values that
matches the predicate f

- f : Predicate
- [init] : The collection to use

> TreeSet.filter(f, [init])

### TreeSet.for_each
Applies f to every item of the collection

- f : Functor

> TreeSet.for_each(f)

### TreeSet.index
Returns the index of the first occurence of item.
Returns -1 if not found.

- item : Item to find
- return : The index of the item (or -1 if not found)

> TreeSet.index(item)

### TreeSet.last_index
Returns the index of the last occurence of item.
Returns -1 if not found.

- item : Item to find
- return : The index of the item (or -1 if not found)

> TreeSet.last_index(item)

### TreeSet.map
Returns a collection with all values mapped
by f

- f : Functor
- [init] : The collection to use

> TreeSet.map(f, [init])

### TreeSet.not_empty
Returns whether it is not empty

- return, Bool

> TreeSet.not_empty()

### TreeSet.pop
Removes an item

- item : Which item to pop
- return : The popped item(s)

> TreeSet.pop(item)

### TreeSet.reduce
Returns the cumulative application of f
to all elements starting by (init, first element)

- f : Functor of 2 parameters
- [init] : The collection to use

> TreeSet.reduce(f, [init])

### TreeSet.sum
Returns the sum of all items.
Returns empty_result if the collection is empty

- empty_result : Result to return if the collection is empty
- return : The sum of all items

> TreeSet.sum([empty_result])

## builtins.Vec
## Vec
Vector data type.
Holds items in a contiguous memory space.
This is the object created with the [a, ...] syntax.

### Vec.add
Pushes back an item

> Vec.add(item)

### Vec.bin_search
Returns the index of item in a sorted collection using
the binary search algorithm.
Returns -1 if not found.

- item : Item to find
- return : The index of the item (or -1 if not found)

> Vec.bin_search(item)

### Vec.empty
Returns whether it is empty
- return, Bool

> Vec.empty()

### Vec.filter
Returns a collection with all values that
matches the predicate f

- f : Predicate
- [init] : The collection to use

> Vec.filter(f, [init])

### Vec.for_each
Applies f to every item of the collection

- f : Functor

> Vec.for_each(f)

### Vec.index
Returns the index of the first occurence of item.
Returns -1 if not found.

- item : Item to find
- return : The index of the item (or -1 if not found)

> Vec.index(item)

### Vec.last_index
Returns the index of the last occurence of item.
Returns -1 if not found.

- item : Item to find
- return : The index of the item (or -1 if not found)

> Vec.last_index(item)

### Vec.map
Returns a collection with all values mapped
by f

- f : Functor
- [init] : The collection to use

> Vec.map(f, [init])

### Vec.not_empty
Returns whether it is not empty

- return, Bool

> Vec.not_empty()

### Vec.pop
Removes an item at a target index (by default, the last item is popped out)

- [index : -1], Int / Range : Where to pop
- return : The popped item(s)

> Vec.pop([index])

### Vec.reduce
Returns the cumulative application of f
to all elements starting by (init, first element)

- f : Functor of 2 parameters
- [init] : The collection to use

> Vec.reduce(f, [init])

### Vec.sort
Sorts the collection inplace

- [cmp] : Custom compare, returns a <=> b, by default a@cmp is used
- return : Returns col

> Vec.sort([cmp])

### Vec.sum
Returns the sum of all items.
Returns empty_result if the collection is empty

- empty_result : Result to return if the collection is empty
- return : The sum of all items

> Vec.sum([empty_result])

## builtins.abs
Returns the absolute value of x

> abs(x)

## builtins.argmax
Returns the index pointing at the maximum of col

- col : Collection
- return, Int : Index of the maximum

> argmax(col)

## builtins.argmin
Returns the index pointing at the minimum of col

- col : Collection
- return, Int : Index of the minimum

> argmin(col)

## builtins.argminmax
Returns the index pointing at the minimum and the maximum of col

- col : Collection
- return, Vec{Int} : Indices of the minimum and the maximum

> argminmax(col)

## builtins.assert
Throws AssertionError if exp is false

- exp, Bool : Expression to test
- [msg], String : Error message

> assert(exp, [msg])

## builtins.autodoc
Generates the auto documentation for multiple modules in
a specified directory

- dirpath, Str : Path to the target directory
- modules, HashMap : [module_name, module_content] pairs to document
- title, Str : Header of the README
- [description, Str] : Description added just after the header in the README
- [verbose, Bool] : If true, displays written files
- [notes, HashMap] : [note_name, note_content] pairs, notes added at
    the bottom of the README
- [lowercase_filenames, Bool] : Whether markdown files are lower cased

> autodoc(dirpath, modules, title, [description], [verbose], [notes], [lowercase_filenames])

## builtins.autodoc_std
Generates the Riddim documentation

- dirpath, Str : Directory where the documentation is generated to

> autodoc_std(dirpath)

## builtins.copy
Shallow copies of obj

- obj : Object to copy
- return : Copied object

> copy(obj)

## builtins.default_collection_bin_search
Returns the index of item in a sorted collection using
the binary search algorithm.
Returns -1 if not found.

- item : Item to find
- return : The index of the item (or -1 if not found)

> default_collection_bin_search(item)

## builtins.default_collection_empty
Returns whether it is empty
- return, Bool

> default_collection_empty()

## builtins.default_collection_filter
Returns a collection with all values that
matches the predicate f

- f : Predicate
- [init] : The collection to use

> default_collection_filter(f, [init])

## builtins.default_collection_for_each
Applies f to every item of the collection

- f : Functor

> default_collection_for_each(f)

## builtins.default_collection_index
Returns the index of the first occurence of item.
Returns -1 if not found.

- item : Item to find
- return : The index of the item (or -1 if not found)

> default_collection_index(item)

## builtins.default_collection_last_index
Returns the index of the last occurence of item.
Returns -1 if not found.

- item : Item to find
- return : The index of the item (or -1 if not found)

> default_collection_last_index(item)

## builtins.default_collection_map
Returns a collection with all values mapped
by f

- f : Functor
- [init] : The collection to use

> default_collection_map(f, [init])

## builtins.default_collection_not_empty
Returns whether it is not empty

- return, Bool

> default_collection_not_empty()

## builtins.default_collection_reduce
Returns the cumulative application of f
to all elements starting by (init, first element)

- f : Functor of 2 parameters
- [init] : The collection to use

> default_collection_reduce(f, [init])

## builtins.default_collection_sort
Sorts the collection inplace

- [cmp] : Custom compare, returns a <=> b, by default a@cmp is used
- return : Returns col

> default_collection_sort([cmp])

## builtins.default_collection_sum
Returns the sum of all items.
Returns empty_result if the collection is empty

- empty_result : Result to return if the collection is empty
- return : The sum of all items

> default_collection_sum([empty_result])

## builtins.doc
Returns the documentation of obj

- obj : Object with documentation
- return, Str : Documentation

> doc(obj)

## builtins.exit
Exits the program

- [code : 0], Int : Exit code

> exit([code])

## builtins.fatal
Prints msg to stderr and exits with exit_code
- msg : Message to print
- [exit_code], Int : Program output

> fatal(msg, [exit_code])

## builtins.filter
Returns a collection with all values that
matches the predicate f

- col : Iterable
- f : Predicate
- [init] : The collection to use

> filter(col, f, [init])

## builtins.for_each
Applies f to every item of the collection

- col : Iterable
- f : Functor

> for_each(col, f)

## builtins.hash
Returns the hash value of obj

- obj : Target
- return, Int : Hash

> hash(obj)

## builtins.input
Reads the next line of stdin.
Throws a RuntimeError if end of file found

> input()

## builtins.iter
Returns the iterator of iterable (begin position)

- iterable : Object with @iter slot
- return : Iterator like object

> iter(iterable)

## builtins.len
Returns the length of col

- col : Object with @len slot
- return, Int : Length

> len(col)

## builtins.map
Returns a collection with all values mapped
by f

- col : Iterable
- f : Functor
- [init] : The collection to use

> map(col, f, [init])

## builtins.max
Returns the maximum value within col

- col : Collection
- return : Maximum

> max(col)

## builtins.min
Returns the minimum value within col

- col : Collection
- return : Minimum

> min(col)

## builtins.minmax
Returns the minimum and the maximum values within col

- col : Collection
- return, Vec : Minimum and maximum values

> minmax(col)

## builtins.next
Returns the next element of iterator (or enditer)

- iterator : Object with @next slot
- return : Next object or enditer

> next(iterator)

## builtins.print
Prints to stdout all arguments

- [args...] : Objects to print

> print([args...])

## builtins.reduce
Returns the cumulative application of f
to all elements starting by (init, first element)

- col : Iterable
- f : Functor of 2 parameters
- [init] : The collection to use

> reduce(col, f, [init])

## builtins.scan
Scans the string content

- content : What to scan
- [args..., Type] : How to scan, can be one of Int, Float, Bool, Str
- return, Vec : Scanned values

> scan(content, [args...])

## builtins.set_default_collection
Sets up the collection type with default collection methods
and attribute variables.

> set_default_collection(type, [is_random_access])

## builtins.sort
Sorts the collection inplace

- col : Random access collection
- [cmp] : Custom compare, returns a <=> b, by default a@cmp is used
- return : Returns col

> sort(col)

## builtins.sum
Returns the sum of all items.
Returns empty_result if the collection is empty

- col : Iterable
- empty_result : Result to return if the collection is empty
- return : The sum of all items

> sum(col, [empty_result])

## builtins.throw
Throws error

- error : Target error

> throw(error)

## builtins.typename
Returns the name of type

- type : Target
- return, Str : Name of type

> typename(type)

## builtins.typeof
Returns the type of obj

- obj : Target
- return, Type : Type of the object

> typeof(obj, [cmp])

## builtins.version
### add
Appends the other string

- s, Str : String to append

> Str.add()

### chr
Returns the character associated to the code

> Str.chr()

### index
> Str.index()

### lower
Transforms the string in lower case characters (inplace)

> Str.lower()

### ord
Returns the code associated to the character

> Str.ord()

### upper
Transforms the string in upper case characters (inplace)

> Str.upper()

