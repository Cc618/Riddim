#include "str.hh"
#include "error.hh"
#include "hash.hh"
#include "int.hh"
#include "null.hh"
#include <cstring>

using namespace std;

Type *Str::class_type = nullptr;

Str::Str(const str_t &data) : Object(Str::class_type), data(data) {}

void Str::init_class_type() {
    class_type = new (nothrow) Type("Str");

    if (!class_type) {
        THROW_MEMORY_ERROR;

        return;
    }

    // @add
    class_type->fn_add = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<Str *>(self);

        if (o->type != Str::class_type) {
            THROW_TYPE_ERROR_PREF("Str.@add", o->type, Str::class_type);

            return nullptr;
        }

        auto result =
            new (nothrow) Str(me->data + reinterpret_cast<Str *>(o)->data);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @cmp
    class_type->fn_cmp = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<Str *>(self);

        int_t res;

        if (o->type != Str::class_type) {
            res = -1;
        } else {
            auto other = reinterpret_cast<Str *>(o)->data;
            res = strcmp(me->data.c_str(), other.c_str());
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
        auto me = reinterpret_cast<Str *>(self);

        auto result = new (nothrow) Str(me->data);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @getattr
    class_type->fn_getattr = [](Object *self, Object *name) -> Object * {
        auto me = reinterpret_cast<Str *>(self);

        if (name->type != Str::class_type) {
            THROW_TYPE_ERROR_PREF("Str.@getattr", name->type, Str::class_type);

            return nullptr;
        }

        auto attr = reinterpret_cast<Str *>(name)->data;

        Object *result = nullptr;

        // Length
        if (attr == "len") {
            result = new (nothrow) Int(me->data.size());
        } else {
            // No such attribute
            THROW_ATTR_ERROR(Str::class_type, attr);

            return nullptr;
        }

        // Check whether the object has been allocated
        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @getitem
    class_type->fn_getitem = [](Object *self, Object *key) -> Object * {
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

    // @len
    class_type->fn_len = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Str *>(self);

        auto result = new (nothrow) Int(me->data.size());

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @mul
    class_type->fn_mul = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<Str *>(self);

        if (o->type != Int::class_type) {
            THROW_TYPE_ERROR_PREF("Str.@mul", o->type, Int::class_type);

            return nullptr;
        }

        int_t multiplier = reinterpret_cast<Int *>(o)->data;

        if (multiplier < 0) {
            THROW_ARITHMETIC_ERROR("*", "Product requires a positive number");

            return nullptr;
        }

        str_t data;
        data.reserve(me->data.size() * multiplier);
        for (size_t i = 0; i < multiplier; ++i)
            data += me->data;

        auto result = new (nothrow) Str(data);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @setitem
    class_type->fn_setitem = [](Object *self, Object *key,
                                Object *value) -> Object * {
        Str *me = reinterpret_cast<Str *>(self);

        // TODO : Slice
        if (key->type == Int::class_type) {
            // TODO index : Index mapping (-1)
            int_t index = reinterpret_cast<Int *>(key)->data;

            // Outside of bounds
            if (index < 0 || index >= me->data.size()) {
                throw_fmt(
                    IndexError,
                    "Index '%lld' outside of bounds for Str of length '%zu'",
                    (long long)index, me->data.size());

                return nullptr;
            }

            str_t val;

            // TODO : Char
            if (value->type == Str::class_type) {
                val = reinterpret_cast<Str *>(value)->data;
            } else
                THROW_TYPE_ERROR_PREF("Str.@setitem", value->type,
                                      Str::class_type);

            me->data =
                me->data.substr(0, index) + val + me->data.substr(index + 1);

            return null;
        } else {
            throw_fmt(TypeError, "Invalid type '%s' to index Str",
                      key->type->name.c_str());

            return nullptr;
        }
    };

    // @str
    class_type->fn_str = [](Object *self) { return self; };
}
