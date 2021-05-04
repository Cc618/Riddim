#include "null.hh"
#include "program.hh"
#include "str.hh"
#include "error.hh"

using namespace std;

Null *null = nullptr;

Type *Null::class_type = nullptr;

void Null::init_class_type() {
    class_type = new (nothrow) Type("Null");

    if (!class_type) {
        THROW_MEMORY_ERROR;

        return;
    }

    // @str
    class_type->fn_str = [](Object *self) -> Object * {
        auto result = Str::New("null");

        if (!result) {
            return nullptr;
        }

        return result;
    };
}

void Null::init_singleton() {
    null = new (nothrow) Null();

    if (!null) {
        THROW_MEMORY_ERROR;

        return;
    }
}

Null::Null() : Object(Null::class_type) {}
