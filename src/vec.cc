#include "vec.hh"
#include "bool.hh"
#include "error.hh"
#include "function.hh"
#include "hash.hh"
#include "int.hh"
#include "methods.hh"
#include "null.hh"
#include "program.hh"
#include "str.hh"
#include <algorithm>
#include <string>

using namespace std;

Type *Vec::class_type = nullptr;

Vec *Vec::empty = nullptr;

size_t Vec::class_hash = 0;

Vec *Vec::New(const vec_t &data) {
    auto self = new (nothrow) Vec(data);

    if (!self) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    NEW_METHOD(Vec, add);
    NEW_METHOD(Vec, pop);

    return self;
}

Vec::Vec(const vec_t &data) : Object(Vec::class_type), data(data) {}

void Vec::init_class_type() {
    class_type = new (nothrow) Type("Vec");
    if (!class_type) {
        THROW_MEMORY_ERROR;
        return;
    }

    class_type->constructor = [](Object *self, Object *args,
                                 Object *kwargs) -> Object * {
        INIT_METHOD(Vec, "Vec");

        CHECK_NOARGS("Vec");
        CHECK_NOKWARGS("Vec");

        auto result = Vec::New();

        // Dispatch error
        if (!result)
            return nullptr;

        return result;
    };

    class_type->fn_traverse_objects = [](Object *self,
                                         const fn_visit_object_t &visit) {
        Vec *me = reinterpret_cast<Vec *>(self);

        for (auto o : me->data) {
            visit(o);
        }

        visit(me->me_add);
        visit(me->me_pop);
    };

    // @hash
    class_type->fn_hash = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Vec *>(self);

        int_t h = class_hash;

        for (auto o : me->data) {
            auto item_hash = o->hash();
            if (!item_hash) {
                return nullptr;
            }

            if (item_hash->type != Int::class_type) {
                THROW_TYPE_ERROR_PREF(o->type->name + ".@hash", item_hash->type, Int::class_type);

                return nullptr;
            }

            h = hash_combine(h, reinterpret_cast<Int*>(item_hash)->data);
        }

        auto result = new (nothrow) Int(h);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @len
    class_type->fn_len = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Vec *>(self);

        auto result = new (nothrow) Int(me->data.size());

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @copy
    class_type->fn_copy = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Vec *>(self);

        auto result = Vec::New(me->data);

        if (!result) {
            return nullptr;
        }

        return result;
    };

    class_type->fn_in = [](Object *self, Object *val) -> Object * {
        auto me = reinterpret_cast<Vec *>(self);

        // TODO : Override equal operator
        bool result =
            find_if(me->data.begin(), me->data.end(), [val](Object *o) -> bool {
                // TODO : Unsafe (no error handling), rm (see todo above)
                return reinterpret_cast<Int *>(o->hash())->data ==
                       reinterpret_cast<Int *>(val->hash())->data;
            }) != me->data.end();

        return result ? istrue : isfalse;
    };

    class_type->fn_mul = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<Vec *>(self);

        if (o->type != Int::class_type) {
            THROW_TYPE_ERROR_PREF("Vec.@mul", o->type, Int::class_type);

            return nullptr;
        }

        int_t multiplier = reinterpret_cast<Int *>(o)->data;

        if (multiplier < 0) {
            THROW_ARITHMETIC_ERROR("*", "Product requires a positive number");

            return nullptr;
        }

        vec_t data;
        data.reserve(me->data.size() * multiplier);
        for (size_t i = 0; i < multiplier; ++i)
            data.insert(data.end(), me->data.begin(), me->data.end());

        auto result = Vec::New(data);

        if (!result) {
            return nullptr;
        }

        return result;
    };

    // @getattr
    class_type->fn_getattr = [](Object *self, Object *name) -> Object * {
        auto me = reinterpret_cast<Vec *>(self);

        if (name->type != Str::class_type) {
            THROW_TYPE_ERROR_PREF("Vec.@getattr", name->type, Str::class_type);

            return nullptr;
        }

        auto attr = reinterpret_cast<Str *>(name)->data;

        Object *result = nullptr;

        // Length
        // TODO : Use AttrObject
        if (attr == "len") {
            result = new (nothrow) Int(me->data.size());
        } else if (attr == "add") {
            return me->me_add;
        } else if (attr == "pop") {
            return me->me_pop;
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

    class_type->fn_getitem = [](Object *self, Object *key) -> Object * {
        auto me = reinterpret_cast<Vec *>(self);

        // TODO : Slice
        if (key->type != Int::class_type) {
            THROW_TYPE_ERROR_PREF("Vec.@getitem", key->type, Int::class_type);

            return nullptr;
        }

        auto idx = reinterpret_cast<Int *>(key)->data;

        // Out of bounds
        if (idx < 0 || idx >= me->data.size()) {
            THROW_OUT_OF_BOUNDS(me->data.size(), idx);

            return nullptr;
        }

        auto result = me->data[idx];

        return result;
    };

    // @str
    class_type->fn_str = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Vec *>(self);

        string result;

        if (me->data.size() == 0)
            result = "[]";
        else {
            result = "[";
            bool isfirst = true;
            for (auto item : me->data) {
                if (isfirst)
                    isfirst = false;
                else
                    result += ", ";

                auto item_str = item->str();

                // Error
                if (!item_str)
                    return nullptr;

                // Check type
                if (item_str->type != Str::class_type) {
                    THROW_TYPE_ERROR_PREF("Vec.@str", item_str->type,
                                          Str::class_type);

                    return nullptr;
                }

                result += reinterpret_cast<Str *>(item_str)->data;
            }

            result += "]";
        }

        auto result_str = new (nothrow) Str(result);

        if (!result_str) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result_str;
    };

    // @setitem
    class_type->fn_setitem = [](Object *self, Object *key,
                                Object *value) -> Object * {
        auto me = reinterpret_cast<Vec *>(self);

        // TODO : Slice
        if (key->type != Int::class_type) {
            THROW_TYPE_ERROR_PREF("Vec.@setitem", key->type, Int::class_type);

            return nullptr;
        }

        auto idx = reinterpret_cast<Int *>(key)->data;

        // Out of bounds
        if (idx < 0 || idx >= me->data.size()) {
            THROW_OUT_OF_BOUNDS(me->data.size(), idx);

            return nullptr;
        }

        me->data[idx] = value;

        return null;
    };
}

void Vec::init_class_objects() {
    empty = Vec::New();

    if (!empty) {
        return;
    }

    Program::add_global(empty);

    class_hash = std::hash<str_t>()("Vec");
}

// --- Methods ---
Object *Vec::me_add_handler(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(Vec, "add");

    CHECK_ARGSLEN(1, "Vec.add");
    CHECK_NOKWARGS("Vec.add");

    me->data.push_back(args_data[0]);

    return null;
}

// TODO : Pop position
Object *Vec::me_pop_handler(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(Vec, "pop");

    CHECK_NOARGS("Vec.pop");
    CHECK_NOKWARGS("Vec.pop");

    if (me->data.empty()) {
        throw_str(IndexError, "Can't pop empty collection");

        return nullptr;
    }

    me->data.pop_back();

    return null;
}
