#pragma once

#include "ast.hh"
#include "module.hh"

// Generates the code of the module describes by the ast
// Returns whether it executed sucessfully
bool gen_module_code(ast::AstModule *astnode, ModuleObject *module);
