#include "str.hh"
#include "error.hh"
#include "hash.hh"
#include "int.hh"
#include "null.hh"
#include "map.hh"
#include "vec.hh"
#include "function.hh"
#include <cstring>

using namespace std;

Type *Str::class_type = nullptr;

Str::Str(const str_t &data) : Object(Str::class_type), data(data) {
    auto self = this;

    NEW_METHOD(Str, index);
}

void Str::init_class_type() {
    class_type = new (nothrow) Type("Str");

    if (!class_type) {
        THROW_MEMORY_ERROR;

        return;
    }

    class_type->fn_traverse_objects = [](Object *self,
                                         const fn_visit_object_t &visit) {
        Str *me = reinterpret_cast<Str *>(self);

        visit(me->me_index);
    };

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
        // TODO : Use AttrObject
        if (attr == "len") {
            result = new (nothrow) Int(me->data.size());
        } else if (attr == "index") {
            result = me->me_index;
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

        if (key->type == Int::class_type) {
            int_t index = get_mod_index(reinterpret_cast<Int *>(key)->data,
                                        me->data.size());

            // Outside of bounds
            if (index < 0 || index >= me->data.size()) {
                THROW_OUT_OF_BOUNDS(me->data.size(), index);

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
            // Slice
            // Collect indices
            auto collect = try_collect_int_iterator(key, -me->data.size(),
                                                    me->data.size());

            if (on_error()) {
                clear_error();

                // Rethrow another one
                throw_fmt(TypeError,
                          "Invalid type '%s' to index Str (must be Int or an "
                          "iterable)",
                          key->type->name.c_str());

                return nullptr;
            }

            str_t result;
            for (auto i : collect) {
                result += me->data[get_mod_index(i, me->data.size())];
            }

            auto ret = new (nothrow) Str(result);

            if (!ret) {
                THROW_MEMORY_ERROR;

                return nullptr;
            }

            return ret;
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
            int_t index = get_mod_index(reinterpret_cast<Int *>(key)->data,
                                        me->data.size());

            // Outside of bounds
            if (index < 0 || index >= me->data.size()) {
                THROW_OUT_OF_BOUNDS(me->data.size(), index);

                return nullptr;
            }

            str_t val;

            if (value->type == Str::class_type) {
                val = reinterpret_cast<Str *>(value)->data;
            } else {
                THROW_TYPE_ERROR_PREF("Str.@setitem", value->type,
                                      Str::class_type);

                return nullptr;
            }

            me->data =
                me->data.substr(0, index) + val + me->data.substr(index + 1);

            return null;
        } else {
            // Slice
            // Collect indices
            auto collect = try_collect_int_iterator(key, -me->data.size(),
                                                    me->data.size());

            if (on_error()) {
                clear_error();

                // Rethrow another one
                throw_fmt(TypeError,
                          "Invalid type '%s' to index Str (must be Int or an "
                          "iterable)",
                          key->type->name.c_str());

                return nullptr;
            }

            // Map modular index to use only positive indinces
            for (auto &i : collect) {
                i = get_mod_index(i, me->data.size());
            }

            // Verify |step| is 1
            for (int i = 1; i < collect.size(); ++i) {
                int prev = collect[i - 1];
                int current = collect[i];

                if (abs(prev - current) != 1) {
                    throw_fmt(IndexError,
                              "Invalid slice for %sStr.@setitem%s, the step "
                              "size must be %s1%s or %s-1%s",
                              C_GREEN, C_NORMAL,
                              C_BLUE, C_NORMAL,
                              C_BLUE, C_NORMAL);

                    return nullptr;
                }

            }

            const auto &[mn_it, mx_it] = minmax_element(collect.begin(), collect.end());

            if (mn_it == collect.end() || mx_it == collect.end()) {
                // Rethrow another one
                throw_fmt(TypeError,
                          "Invalid type '%s' to index Str (must be Int or an "
                          "iterable)",
                          key->type->name.c_str());

                return nullptr;
            }

            int_t mn = *mn_it;
            int_t mx = *mx_it;

            str_t val;

            if (value->type == Str::class_type) {
                val = reinterpret_cast<Str *>(value)->data;
            } else {
                THROW_TYPE_ERROR_PREF("Str.@setitem", value->type,
                                      Str::class_type);

                return nullptr;
            }

            me->data =
                me->data.substr(0, mn) + val + me->data.substr(mx + 1);

            return null;
        }
    };

    // @str
    class_type->fn_str = [](Object *self) { return self; };
}

// --- Methods ---
Object *Str::me_index_handler(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(Str, "index");

    CHECK_ARGSLEN(1, "Str.index");
    CHECK_NOKWARGS("Str.index");

    auto target = args_data[0];

    if (target->type != Str::class_type) {
        THROW_TYPE_ERROR_PREF("Str.index", target->type, Str::class_type);
    }

    auto s = reinterpret_cast<Str*>(target)->data;
    auto idx = me->data.find(s);
    if (idx >= me->data.size()) idx = -1;

    auto result = new (nothrow) Int(idx);

    if (!result) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    return result;
}
