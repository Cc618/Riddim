#pragma once

// Module IO

#include "module.hh"
#include "driver.hh"
#include "utils.hh"
#include "error.hh"
#include <iostream>

// Generates a module object from an AST
Module *gen_module(Driver &driver);

// Parses a source file, returns its module object
Module *parse_module(str_t module_path);

// Loads a module from its special name (path.to.mod -> /path/to/mod.rid)
// - current_path : The path of the module that loads the new module (where
// there is the use keyword), used for search path
Module *load_module(const str_t &name, const str_t &current_path);
