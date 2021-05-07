#include "vec.hh"
#include "bool.hh"
#include "builtins.hh"
#include "error.hh"
#include "function.hh"
#include "hash.hh"
#include "int.hh"
#include "iterator.hh"
#include "methods.hh"
#include "null.hh"
#include "program.hh"
#include "str.hh"
#include <algorithm>
#include <string>

using namespace std;

DynamicType *Vec::class_type = nullptr;

Vec *Vec::empty = nullptr;

size_t Vec::class_hash = 0;

Vec *Vec::New(const vec_t &data) {
    auto self = new (nothrow) Vec(data);

    if (!self) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    DynamicObject::init(self);

    if (on_error())
        return nullptr;

    return self;
}

Vec::Vec(const vec_t &data) : DynamicObject(Vec::class_type), data(data) {}

void Vec::init_class_type() {
    class_type = DynamicType::New("Vec");

    if (!class_type) {
        return;
    }

    class_type->constructor = [](Object *self, Object *args,
                                 Object *kwargs) -> Object * {
        INIT_METHOD(Vec, "Vec");

        CHECK_NOKWARGS("Vec");

        Vec *result = nullptr;
        if (args_data.empty()) {
            result = Vec::New();
        } else if (args_data.size() == 1) {
            // Construct from iterable
            auto iter = args_data[0]->iter();

            if (!iter) {
                THROW_ARGUMENT_ERROR("Vec.@new", "iterable", "Requires an iterable object");

                return nullptr;
            }

            vector<Object *> collect;
            Object *obj = nullptr;
            while (1) {
                obj = iter->next();

                if (!obj) {
                    return nullptr;
                }

                if (obj == enditer) break;

                collect.push_back(obj);
            }

            result = Vec::New(collect);
        } else {
            THROW_ARGUMENT_ERROR("Vec.@new", "length", "0 or 1 arguments required");
        }

        // Dispatch error
        if (!result)
            return nullptr;

        return result;
    };

    class_type->fn_traverse_objects = [](Object *self,
                                         const fn_visit_object_t &visit) {
        // Override and call super
        DynamicType::default_traverse_objects(self, visit);

        if (on_error())
            return;

        Vec *me = reinterpret_cast<Vec *>(self);

        for (auto o : me->data) {
            visit(o);
        }
    };

    // @add
    class_type->fn_add = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<Vec *>(self);

        if (o->type != Vec::class_type) {
            THROW_TYPE_ERROR_PREF("Vec.@add", o->type, Vec::class_type);

            return nullptr;
        }

        // PODs are copied
        auto odata = reinterpret_cast<Vec *>(o)->data;
        vec_t newdata(me->data.size() + odata.size());

        for (int i = 0; i < me->data.size(); ++i) {
            auto item = me->data[i];

            newdata[i] = is_pod_object(item) ? item->copy() : item;
        }

        for (int i = 0; i < odata.size(); ++i) {
            auto item = odata[i];

            newdata[me->data.size() + i] =
                is_pod_object(item) ? item->copy() : item;
        }

        auto result = new (nothrow) Vec(newdata);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @cmp
    class_type->fn_cmp = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<Vec *>(self);

        int_t res = 0;

        if (o->type != Vec::class_type) {
            res = -1;
        } else {
            // Algorithm :
            // - If not the same size, return the size difference
            // - Return the difference between the first different
            // item pair (where me[i] != other[i]) or 0 if none
            auto other = reinterpret_cast<Vec *>(o)->data;

            if (me->data.size() != other.size()) {
                res = me->data.size() - other.size();
            } else {
                for (size_t i = 0; i < me->data.size(); ++i) {
                    // Int type verified
                    auto cmp = me->data[i]->cmp(other[i]);

                    // Error
                    if (!cmp) {
                        return nullptr;
                    }

                    if (cmp) {
                        res = reinterpret_cast<Int *>(cmp)->data;
                        break;
                    }
                }
            }
        }

        auto result = new (nothrow) Int(res);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
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
                THROW_TYPE_ERROR_PREF(o->type->name + ".@hash", item_hash->type,
                                      Int::class_type);

                return nullptr;
            }

            h = hash_combine(h, reinterpret_cast<Int *>(item_hash)->data);
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

    // @in
    class_type->fn_in = [](Object *self, Object *val) -> Object * {
        auto me = reinterpret_cast<Vec *>(self);

        auto it = me->data.begin();
        for (; it != me->data.end(); ++it) {
            auto threeway = (*it)->cmp(val);

            if (!threeway)
                return nullptr;

            // Int type guaranted
            if (reinterpret_cast<Int *>(threeway)->data == 0)
                break;
        }

        bool result = it != me->data.end();

        return result ? istrue : isfalse;
    };

    // @iter
    class_type->fn_iter = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Vec *>(self);

        auto iter = new (nothrow) Iterator(
            [](Iterator *it) -> Object * {
                int_t &i = reinterpret_cast<int_t &>(it->custom_data);
                Vec *me = reinterpret_cast<Vec *>(it->collection);

                if (i >= me->data.size())
                    return enditer;

                auto result = me->data[i];

                ++i;

                return result;
            },
            me, 0);

        if (!iter) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return iter;
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

        vec_t data(me->data.size() * multiplier);
        for (size_t i = 0; i < data.size(); ++i) {
            auto item = me->data[i % me->data.size()];

            data[i] = is_pod_object(item) ? item->copy() : item;
        }

        auto result = Vec::New(data);

        if (!result) {
            return nullptr;
        }

        return result;
    };

    class_type->fn_getitem = [](Object *self, Object *key) -> Object * {
        auto me = reinterpret_cast<Vec *>(self);

        if (key->type == Int::class_type) {
            auto len = me->data.size();
            auto idx = get_mod_index(reinterpret_cast<Int *>(key)->data, len);

            // Out of bounds
            if (idx < 0 || idx >= len) {
                THROW_OUT_OF_BOUNDS(me->data.size(), idx);

                return nullptr;
            }

            auto result = me->data[idx];

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
                          "Invalid type '%s' to index Vec (must be Int or an "
                          "iterable)",
                          key->type->name.c_str());

                return nullptr;
            }

            vector<Object *> result;
            for (auto i : collect) {
                result.push_back(me->data[get_mod_index(i, me->data.size())]);
            }

            auto ret = new (nothrow) Vec(result);

            if (!ret) {
                THROW_MEMORY_ERROR;

                return nullptr;
            }

            return ret;
        }
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

        auto result_str = Str::New(result);

        if (!result_str) {
            return nullptr;
        }

        return result_str;
    };

    // @setitem
    class_type->fn_setitem = [](Object *self, Object *key,
                                Object *value) -> Object * {
        auto me = reinterpret_cast<Vec *>(self);

        if (key->type == Int::class_type) {
            auto idx = get_mod_index(reinterpret_cast<Int *>(key)->data,
                                     me->data.size());

            // Out of bounds
            if (idx < 0 || idx >= me->data.size()) {
                THROW_OUT_OF_BOUNDS(me->data.size(), idx);

                return nullptr;
            }

            me->data[idx] = value;

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
                          "Invalid type '%s' to index Vec (must be Int or an "
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
                              "Invalid slice for %sVec.@setitem%s, the step "
                              "size must be %s1%s or %s-1%s",
                              C_GREEN, C_NORMAL, C_BLUE, C_NORMAL, C_BLUE,
                              C_NORMAL);

                    return nullptr;
                }
            }

            const auto &[mn_it, mx_it] =
                minmax_element(collect.begin(), collect.end());

            if (mn_it == collect.end() || mx_it == collect.end()) {
                // Rethrow another one
                throw_fmt(TypeError,
                          "Invalid type '%s' to index Vec (must be Int or an "
                          "iterable)",
                          key->type->name.c_str());

                return nullptr;
            }

            int_t mn = *mn_it;
            int_t mx = *mx_it;

            str_t val;

            vector<Object *> newdata;

            for (int i = 0; i < mn; ++i)
                newdata.push_back(me->data[i]);

            newdata.push_back(value);

            for (int i = mx + 1; i < me->data.size(); ++i)
                newdata.push_back(me->data[i]);

            me->data = newdata;

            return null;
        }
    };
}

void Vec::init_class_objects() {
    empty = Vec::New();

    if (!empty) {
        return;
    }

    Program::add_global(empty);

    class_hash = std::hash<str_t>()("Vec");

    NEW_METHOD(Vec, add);
    NEW_METHOD(Vec, pop);
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
