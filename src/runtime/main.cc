#include "driver.hh"
#include "error.hh"
#include "frame.hh"
#include "init.hh"
#include "interpreter.hh"
#include "modio.hh"
#include "module.hh"
#include "object.hh"
#include "program.hh"
#include "utils.hh"
#include <iostream>

using namespace std;

void help(ostream &os = cout) {
    os << "usage:" << endl;
    os << "riddim <FILE>        Execute <FILE>" << endl;
    os << "riddim --help        Shows this help" << endl;
    os << "riddim --version     Displays current Riddim version" << endl;
}

int main(int argc, char *argv[]) {
    int res = 0;

    // Invalid arguments
    if (argc != 2) {
        help(cerr);

        return -1;
    }

    string arg1 = argv[1];

    // Special commands
    if (arg1 == "--version") {
        cout << "Riddim " << RID_VERSION << endl;

        return 0;
    } else if (arg1 == "--help") {
        help();

        return 0;
    }

    string main_module_path = abs_path(arg1);

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
    } catch (...) {
    }

    // Something went wrong
    if (!success) {
        if (on_error()) {
            if (Program::instance->current_error->type == ExitError) {
                res = Program::instance->exit_code;
            } else {
                // Display as much information as possible
                if (Program::instance->trace) {
                    Program::instance->trace->dump();

                    dump_error();
                }
                else if (!Program::instance->errout.str().empty())
                    cerr << Program::instance->errout.str();
                else
                    dump_error();

                res = -1;
            }
        }
    }

    end_program();

    return res;
}
