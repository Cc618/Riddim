#include "str.hh"
#include "error.hh"
#include "int.hh"
#include "hash.hh"

using namespace std;

Type *Str::class_type = nullptr;

Str::Str(const str_t &data) : Object(Str::class_type), data(data) {}

void Str::init_class_type() {
    class_type = new (nothrow) Type("Str");

    if (!class_type) {
        THROW_MEMORY_ERROR;

        return;
    }

    // @index
    class_type->fn_index = [](Object *self, Object *key) -> Object* {
        Str *me = reinterpret_cast<Str *>(self);

        // TODO : Slice
        if (key->type == Int::class_type) {
            int_t index = reinterpret_cast<Int *>(key)->data;

            // Outside of bounds
            if (index < 0 || index >= me->data.size()) {
                throw_fmt(
                    IndexError,
                    "Index '%lld' outside of bounds for Str of length '%zu'",
                    (long long)index, me->data.size());

                return nullptr;
            }

            // Build string with one char
            auto result = new (nothrow) Str(str_t(1, me->data[index]));

            if (!result) {
                THROW_MEMORY_ERROR;

                return nullptr;
            }

            return result;
        } else {
            throw_fmt(TypeError, "Invalid type '%s' to index Str",
                      key->type->name.c_str());

            return nullptr;
        }
    };

    // @hash
    class_type->fn_hash = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Str *>(self);

        auto result = new (nothrow) Int(hash_str(me->data));

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };


    // @str
    class_type->fn_str = [](Object *self) { return self; };
}
