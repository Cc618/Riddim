#include "int.hh"
#include "error.hh"
#include "hash.hh"
#include "str.hh"

using namespace std;

Type *Int::class_type = nullptr;

Int::Int(const int_t &data) : Object(Int::class_type), data(data) {}

void Int::init_class_type() {
    class_type = new (nothrow) Type("Int");

    if (!class_type) {
        THROW_MEMORY_ERROR;

        return;
    }

    // @hash
    class_type->fn_hash = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Int *>(self);

        auto result = new (nothrow) Int(hash_int(me->data));

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @str
    class_type->fn_str = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Int *>(self);

        // TODO : Unicode
        auto result = new (nothrow) Str(to_string(me->data));

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };
}
