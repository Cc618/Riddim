# Errors
<!-- TODO : Abstract -->
<!-- TODO : Riddim interface with try catch... -->
<!-- TODO : C++ interface -->

## Types
| Type | Throws when |
| ---- | ----------- |
| ArithmeticError | Zero division... |
| AssertError | False assertion. |
| ImportError | Import not found. |
| IndexError | Out of bounds / key not found on a collection. |
| InternalError | An error thrown only in the C++ API (example : invalid format on throw error). This error can be thrown outside of the Riddim code execution. |
| IterError | Invalid use of iterator. |
| MemoryError | Out of memory. |
| NameError | Local / attribute / global variable not found. |
| NotImplementedError | This method is not implemented yet. |
| RecursionError | Max recursion depth reached |
| TypeError | Invalid type. |

## Files
src/error.{cc,hh}
