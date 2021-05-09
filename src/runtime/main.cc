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
    // TODO C : WIP
    str_t stdpath = RID_STD_PATH;
    cout << "RID_STD_PATH: " << stdpath << endl;

    // TODO C : WIP
#ifdef DEBUG
    cout << "DEBUG" << endl;
#endif

    return 0;

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

    // Interpret code if no errors
    bool success = false;
    try {
        if (module && interpret_program(module))
            success = true;
    } catch (...) {}

    // Something went wrong
    if (!success) {
        if (on_error()) {
            // Display as much information as possible
            if (Program::instance->trace)
                Program::instance->trace->dump();
            else if (!Program::instance->errout.str().empty())
                cerr << Program::instance->errout.str();
            else
                dump_error();
        }

        res = -1;
    }

    end_program();

    return res;
}
