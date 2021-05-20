# Riddim
Riddim is an interpreted language designed for algorithms and data structures.
Riddim is written in C++ and uses Flex / Bison tools.

## Overview
- Look at some [examples](examples/README.md).
- Take a look at [Riddim's syntax](doc/syntax.md).

## Configure / build / install
Here is the single command to install Riddim :
```sh
./configure.sh && sudo scripts/install.sh
```

### Configuration
<!-- TODO : Link to doc -->
The [configure.sh](configure.sh) script can be modified for a custom config
(to install riddim in a custom directory / build in debug mode for example) :
```sh
./configure.sh
```

This will create the build folder with the Makefile.

### Build
After the configuration, the Makefile is located in the build folder.
```sh
cd build && make -j 4
```

**build/src/riddim** is the output of the build.

### Installation
After the configuration script executed, it is possible to build and install
Riddim with the script [scripts/install.sh](scripts/install.sh).

Note that by default, the outputs will be copied to system directories so
a root privilege may be necessary.

```sh
# You can omit sudo if you configured a non system directory
sudo scripts/install.sh
```

### Uninstallation
There is also an uninstallation script.
If you used a custom configuration, source
it before uninstalling riddim.

```sh
# If you have a custom configuration
source configure.sh
# sudo may be useless if you have a custom configuration
sudo scripts/uninstall.sh
```

## Documentation
The documentation can be found [HERE](doc).
