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

    // @add
    class_type->fn_add = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<Int *>(self);

        if (o->type != Int::class_type) {
            THROW_TYPE_ERROR_PREF("Int.@add", o->type, Int::class_type);

            return nullptr;
        }

        auto result =
            new (nothrow) Int(me->data + reinterpret_cast<Int *>(o)->data);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @cmp
    class_type->fn_cmp = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<Int *>(self);

        int_t res;

        if (o->type != Int::class_type) {
            res = -1;
        } else {
            auto other = reinterpret_cast<Int *>(o)->data;
            res = me->data > other ? 1 : me->data < other ? -1 : 0;
        }

        auto result = new (nothrow) Int(res);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @copy
    class_type->fn_copy = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Int *>(self);

        auto result = new (nothrow) Int(me->data);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @div
    class_type->fn_div = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<Int *>(self);

        if (o->type != Int::class_type) {
            THROW_TYPE_ERROR_PREF("Int.@div", o->type, Int::class_type);

            return nullptr;
        }

        auto odata = reinterpret_cast<Int *>(o)->data;

        if (odata == 0) {
            THROW_ARITHMETIC_ERROR("/", "Division by 0");

            return nullptr;
        }

        auto result =
            new (nothrow) Int(me->data / odata);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

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

    // @mod
    class_type->fn_mod = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<Int *>(self);

        if (o->type != Int::class_type) {
            THROW_TYPE_ERROR_PREF("Int.@mod", o->type, Int::class_type);

            return nullptr;
        }

        auto odata = reinterpret_cast<Int *>(o)->data;

        if (odata == 0) {
            THROW_ARITHMETIC_ERROR("%", "Modulo by 0");

            return nullptr;
        }

        auto result =
            new (nothrow) Int(me->data % odata);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @mul
    class_type->fn_mul = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<Int *>(self);

        if (o->type != Int::class_type) {
            THROW_TYPE_ERROR_PREF("Int.@mul", o->type, Int::class_type);

            return nullptr;
        }

        auto result =
            new (nothrow) Int(me->data * reinterpret_cast<Int *>(o)->data);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @neg
    class_type->fn_neg = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Int *>(self);

        auto result =
            new (nothrow) Int(-me->data);

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

    // @sub
    class_type->fn_sub = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<Int *>(self);

        if (o->type != Int::class_type) {
            THROW_TYPE_ERROR_PREF("Int.@sub", o->type, Int::class_type);

            return nullptr;
        }

        auto result =
            new (nothrow) Int(me->data - reinterpret_cast<Int *>(o)->data);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };
}
