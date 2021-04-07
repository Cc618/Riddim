#include "driver.hh"
#include "error.hh"
#include "init.hh"
#include "object.hh"
#include "codegen.hh"
#include "module.hh"
#include "frame.hh"
#include <iostream>

using namespace std;

// // TODO : Multiple modules (files)
// // Interprets the ast in the driver
// int interpret_ast(Driver &driver) {
//     // Take ownership of the ast
//     auto module = driver.module;
//     driver.module = nullptr;

//     // Generate code
//     // module->debug();

//     // Delete ast (all nodes)
//     delete module;

//     // Interpret code
// }

Module *gen_module(Driver &driver) {
    // Take ownership of the ast
    auto ast = driver.module;
    driver.module = nullptr;

    // TODO : Module name not file path
    // Generate code
    auto module = Module::New(driver.file, driver.file);
    gen_module_code(ast, module);

    // TODO : Handle errors

    // Delete all ast nodes
    delete ast;

    return 0;
}

int main(int argc, char *argv[]) {
    // // --- Objects ---
    // init_program();

    // // Check for initialization errors
    // if (on_error()) {
    //     cerr << "Failed to initialize the program" << endl;

    //     dump_error();
    // }

    // try {
    //     testObjects();
    // } catch (...) {
    //     throw_str(InternalError, "Internal error");
    // }

    // if (on_error()) {
    //     cerr << "Uncaught error" << endl;

    //     dump_error();
    // }

    // end_program();

    // cout << "Done" << endl;

    // return 0;

    int res = 0;
    Driver driver;

    // Invalid arguments
    if (argc != 2) {
        cerr << "usage: ./riddim <FILE>" << endl;

        return -1;
    }

    // Init VM
    init_program();

    // Check for initialization errors
    if (on_error()) {
        cerr << "Error : Failed to initialize Riddim Virtual Machine" << endl;

        dump_error();

        return -1;
    }

    // Parse file
    string file = argv[1];
    res = driver.parse(file);

    if (res) {
        cerr << "Error : Failed to parse file " << file << endl;

        return res;
    }

    try {
        // Generate code from the AST
        auto module = gen_module(driver);

        // The exception is thrown to
        if (!module) goto on_internal_error;

        // Interpret code

        // TODO
        // testObjects();

        // Error but we don't want to go within the catch block
    on_internal_error:;
    } catch (...) {
        throw_str(InternalError, "Internal error");
    }

    if (on_error()) {
        cerr << "Error : Uncaught error" << endl;

        dump_error();

        res = -1;
    }

    end_program();

    return res;
}
