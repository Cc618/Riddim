#include "driver.hh"
// TODO
#include "object.hh"
#include "init.hh"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
    init_program();
    testObjects();
    cout << "Done" << endl;
    end_program();

    return 0;

    /*
    int res;
    Driver driver;

    if (argc != 2) {
        cerr << "usage: ./riddim <FILE>" << endl;

        res = -1;
    } else {
        cout << "Parsing..." << endl;
        res = driver.parse(argv[1]);

        if (!res) {
            auto module = driver.module;
            driver.module = nullptr;

            cout << "Parsed" << endl;

            // TODO : Gen code
            module->debug();

            delete module;

            // TODO : Interpret code
            cout << "Deleted" << endl;
        }
    }

    return res;
    */
}
