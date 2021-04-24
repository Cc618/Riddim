#include "driver.hh"
#include "error.hh"
#include "frame.hh"
#include "init.hh"
#include "interpreter.hh"
#include "module.hh"
#include "object.hh"
#include "program.hh"
#include "utils.hh"
#include "modio.hh"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
    int res = 0;

    // Invalid arguments
    if (argc != 2) {
        cerr << "usage: ./riddim <FILE>" << endl;

        return -1;
    }

    string main_module_path = abs_path(argv[1]);

    // Init VM
    init_program();

    Program::instance->main_module_path = main_module_path;

    // Check for initialization errors
    if (on_error()) {
        cerr << "Error : Failed to initialize Riddim Virtual Machine" << endl;

        dump_error();

        return -1;
    }

    // Parse main module to a module object
    auto module = parse_module(main_module_path);

    // TODO F : Try catch
    // Interpret code if no errors
    if (!module || !interpret_program(module)) {
        res = -1;
    }

    end_program();

    return res;
}
