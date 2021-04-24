#include "modio.hh"
#include "codegen.hh"
#include "utils.hh"

using namespace std;

Module *gen_module(Driver &driver) {
    // Take ownership of the ast
    auto ast = driver.module;
    driver.module = nullptr;

    auto file_path = driver.file;
    auto mod_name = module_name(file_path);

    if (mod_name == "") {
        cerr << "Invalid module path" << endl;

        return nullptr;
    }

    // Generate code
    auto module = Module::New(mod_name, file_path);

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

Module *parse_module(str_t module_path) {
    Driver driver;
    module_path = abs_path(module_path);

    // Parse main module
    int res = driver.parse(module_path);

    if (res) {
        throw_fmt(ImportError, "Failed to parse file %s%s%s", C_BLUE,
                  module_path.c_str(), C_NORMAL);

        if (driver.module) {
            driver.module = nullptr;
        }

        return nullptr;
    }

    try {
        // Generate code from the AST
        auto module = gen_module(driver);

        if (!module) {
            throw_fmt(ImportError, "%s%s%s: Syntax error", C_BLUE,
                      module_path.c_str(), C_NORMAL);

            return nullptr;
        }

        return module;
    } catch (...) {
        throw_str(InternalError, "Internal error");
    }

    return nullptr;
}

Module *load_module(const str_t &name) {
    // TODO A : Parse . + super + path
    const str_t path = "./" + name + ".rid";

    return parse_module(path);
}