#include "bool.hh"
#include "error.hh"
#include "str.hh"
#include "program.hh"

using namespace std;

Type *Bool::class_type = nullptr;

Bool *istrue = nullptr;
Bool *isfalse = nullptr;

Bool::Bool(bool data) : Object(Bool::class_type), data(data) {}

void Bool::init_class_type() {
    class_type = new (nothrow) Type("Bool");
    if (!class_type) {
        THROW_MEMORY_ERROR;
        return;
    }

    class_type->fn_str = [](Object *self) -> Object * {
        Bool *me = reinterpret_cast<Bool *>(self);

        auto s = new (nothrow) Str(me->data ? "true" : "false");

        if (!s) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return s;
    };
}

void Bool::init_class_objects() {
    istrue = new (nothrow) Bool(true);

    if (!istrue) {
        THROW_MEMORY_ERROR;

        return;
    }

    isfalse = new (nothrow) Bool(false);

    if (!isfalse) {
        THROW_MEMORY_ERROR;

        return;
    }

    Program::instance->globals.push_back(istrue);
    Program::instance->globals.push_back(isfalse);
}
