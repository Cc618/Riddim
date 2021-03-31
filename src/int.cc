#include "int.hh"
#include "error.hh"

using namespace std;

Type *Int::class_type = nullptr;

Int::Int(const int_t &data) : Object(Int::class_type), data(data) {}

void Int::init_class_type() {
    class_type = new (nothrow) Type("Int");

    if (!class_type) {
        THROW_MEMORY_ERROR;

        return;
    }

    // TODO
    // class_type->fn_str
}
