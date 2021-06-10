# Build / install configuration
Riddim can be customized with the **configure.sh** script located at the
root of the project.
In this script, multiple variables can be overriden like the location
of the riddim binary.
All variables are commented and described in this script.

## Debug
When developing Riddim, it can be useful to print some informations.
It is possible to define some definitions to display those messages.

### Flags (defines)
These definitions can be defined in the configure.sh script, the variable
must be exported with this syntax :
```sh
export RID_$DEBUGFLAGNAME=1

# Will define the DEBUG definition
export RID_DEBUG=1
```

| Name | C / CMake | Description |
| ---- | --------- | ----------- |
| DEBUG | C | Debug mode. |
| DEBUG_ERRORS | C | Show when an error is thrown and display its type. |
| DEBUG_GC | C | Prints when the garbage collector is running and how many objects are alive |
| DEBUG_PARSER_CONFLICTS | CMake | Prints shift / reduce or reduce / reduce conflicts of the parser at compile time |
