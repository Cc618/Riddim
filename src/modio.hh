#pragma once

// Module IO

#include "module.hh"
#include "driver.hh"
#include "utils.hh"
#include "error.hh"
#include <iostream>

Module *gen_module(Driver &driver);

Module *parse_module(str_t module_path);
