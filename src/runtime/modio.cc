#include "modio.hh"
#include "codegen.hh"
#include "program.hh"
#include "utils.hh"
#include <algorithm>
#include <filesystem>
#include <sstream>

using namespace std;
namespace fs = std::filesystem;

Module *gen_module(Driver &driver) {
    // Take ownership of the ast
    auto ast = driver.module;
    driver.module = nullptr;

    auto file_path = driver.file;
    auto mod_name = module_name(file_path);

    if (mod_name == "") {
        throw_fmt(ImportError, "Invalid module path");

        return nullptr;
    }

    // Generate code
    auto module = Module::New(mod_name, file_path);

    if (!module) {
        throw_fmt(ImportError, "Can't allocate module %s%s%s", C_BLUE,
                  file_path.c_str(), C_NORMAL);

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

Module *parse_module(str_t module_path, const str_t &module_name, bool isbuiltin) {
    auto new_module_path = abs_path(module_path);

    if (!is_file(new_module_path)) {
        throw_fmt(ImportError, "File %s%s%s not found", C_BLUE,
                  module_path.c_str(), C_NORMAL);

        return nullptr;
    }

    module_path = new_module_path;

    // Test whether this module is already loaded
    auto it = Program::instance->modules.find(module_path);
    if (it != Program::instance->modules.end()) {
        return it->second;
    }

    // Parse main module
    Driver driver;
    int res = driver.parse(module_path);

    if (res) {
        auto msg = Program::instance->errout.str();
        Program::instance->errout.clear();

        if (msg.empty())
            throw_fmt(ImportError, "%s%s%s: Syntax error", C_BLUE,
                      module_path.c_str(), C_NORMAL);
        else
            throw_fmt(ImportError, "\n%s", msg.c_str());

        if (driver.module) {
            driver.module = nullptr;
        }

        return nullptr;
    }

    try {
        // Generate code from the AST
        auto module = gen_module(driver);

        if (!module) {
            auto msg = Program::instance->errout.str();
            Program::instance->errout.clear();

            if (msg.empty())
                throw_fmt(ImportError, "%s%s%s: Syntax error", C_BLUE,
                          module_path.c_str(), C_NORMAL);
            else
                throw_fmt(ImportError, "\n%s", msg.c_str());

            return nullptr;
        }

        Program::add_module(module);

        if (isbuiltin) {
            Program::on_std_module_loaded(module_name, module);

            if (on_error()) {
                return nullptr;
            }
        }

        return module;
    } catch (...) {
        throw_str(InternalError, "Internal error");
    }

    return nullptr;
}

Module *load_module(const str_t &name, const str_t &current_path) {
    stringstream name_stream(name);
    vector<str_t> dirs;

    // Not really a line but a directory
    str_t line;
    while (getline(name_stream, line, '.')) {
        dirs.push_back(line);
    }

    // Parse path, replace . by / and super by ..
    str_t parsed_path;
    for (int i = 0; i < dirs.size(); ++i) {
        str_t item = dirs[i];

        if (item == "super")
            item = "..";

        parsed_path += "/" + item;
    }

    // Current directory
    auto mod_dir = current_path.empty() ? "" : dir_path(current_path);

    // Std lib modules directory
    auto std_dir = Program::std_path;

    // All directories that can contain the module
    // The first one has the highest priority
    // This is similar to PYTHONPATH
    vector<str_t> search_path;

    if (!mod_dir.empty()) {
        search_path.push_back(mod_dir);
    }

    auto std_dir_index = search_path.size();
    search_path.push_back(std_dir);

    int i = 0;
    for (auto dir : search_path) {
        str_t path = dir + parsed_path + ".rid";

        if (is_file(path)) {
            return parse_module(path, name, i == std_dir_index);
        }

        ++i;
    }

    throw_fmt(ImportError, "Module %s%s%s not found", C_BLUE, name.c_str(),
              C_NORMAL);

    return nullptr;
}
