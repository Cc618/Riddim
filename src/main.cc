#include "codegen.hh"
#include "driver.hh"
#include "error.hh"
#include "frame.hh"
#include "init.hh"
#include "interpreter.hh"
#include "module.hh"
#include "object.hh"
#include "program.hh"
#include <iostream>

using namespace std;

Module *gen_module(Driver &driver) {
    // Take ownership of the ast
    auto ast = driver.module;
    driver.module = nullptr;

    // TODO : Module name not file path
    // TODO : !module error handling
    // Generate code
    auto module = Module::New(driver.file, driver.file);

    // Empty
    if (!ast)
        return module;

    bool success = gen_module_code(ast, module);

    // Delete all ast nodes
    delete ast;

    // Errors
    if (!success) {
        delete module;

        return nullptr;
    }

    return module;
}

int main(int argc, char *argv[]) {
    int res = 0;
    Driver driver;

    // Invalid arguments
    if (argc != 2) {
        cerr << "usage: ./riddim <FILE>" << endl;

        return -1;
    }

    string main_module_path = argv[1];

    // Init VM
    init_program();

    Program::instance->main_module_path = main_module_path;

    // Check for initialization errors
    if (on_error()) {
        cerr << "Error : Failed to initialize Riddim Virtual Machine" << endl;

        dump_error();

        return -1;
    }

    // Parse main module
    res = driver.parse(main_module_path);

    if (res) {
        cerr << "Error : Failed to parse file " << main_module_path << endl;

        if (driver.module) {
            delete driver.module;

            driver.module = nullptr;
        }

        goto on_parse_error;
    }

    try {
        // TODO
        // driver.module->debug();

        // Generate code from the AST
        auto module = gen_module(driver);

        // The exception is thrown to
        if (!module)
            goto on_internal_error;

        // TODO
        cout << "Interpreting code" << endl;

        // Interpret code
        if (!interpret_program(module->code)) {
            res = -1;

            goto on_internal_error;
        }

        // Error but we don't want to go within the catch block
    on_internal_error:;
    } catch (...) {
        throw_str(InternalError, "Internal error");
    }

on_parse_error:;
    end_program();

    return res;
}
