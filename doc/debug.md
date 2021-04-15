# Debug
When developing Riddim, it can be useful to print some informations.
It is possible to define some definitions to display those messages.

## Flags (defines)
These definitions are added to the CMakeLists.txt file in the src directory.
If the type is C, the constant is a C definition (#define), otherwise it is a CMake variable with value 1.

| Name | C / CMake | Description |
| ---- | --------- | ----------- |
| DEBUG | C | Debug mode. |
| DEBUG_ERRORS | C | Show when an error is thrown and display its type. |
| DEBUG_GC | C | Prints when the garbage collector is running and how many objects are alive |
| DEBUG_PARSER_CONFLICTS | CMake | Prints shift / reduce or reduce / reduce conflicts of the parser at compile time |
