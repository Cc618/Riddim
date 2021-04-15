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

    // TODO : Absolute
    auto file_path = driver.file;

    // TODO : Module name not file path
    // Generate code
    auto module = Module::New(file_path, file_path);

    if (!module) {
        cerr << "Can't allocate module " << file_path << endl;

        return nullptr;
    }

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
        // Generate code from the AST
        auto module = gen_module(driver);

        if (!module) {
            res = -1;

            goto on_error;
        }

        // Interpret code
        if (!interpret_program(module)) {
            res = -1;

            goto on_error;
        }

    on_error:;
    } catch (...) {
        throw_str(InternalError, "Internal error");
    }

on_parse_error:;
    end_program();

    return res;
}
