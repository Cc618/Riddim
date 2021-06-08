#include "segtree.hh"
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
#include "range.hh"
#include "str.hh"
#include <algorithm>
#include <string>

using namespace std;

DynamicType *SegTree::class_type = nullptr;

size_t SegTree::class_hash = 0;

SegTree *SegTree::New(const vec_t &data, Object *functor) {
    auto self = new (nothrow) SegTree(data, functor);

    if (!self) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    DynamicObject::init(self);

    if (on_error())
        return nullptr;

    return self;
}

// TODO A
SegTree::SegTree(const vec_t &data, Object *functor)
    : DynamicObject(SegTree::class_type), data(data), functor(functor) {}

void SegTree::init_class_type() {
    class_type = DynamicType::New("SegTree");

    if (!class_type) {
        return;
    }

    class_type->constructor = [](Object *self, Object *args,
                                 Object *kwargs) -> Object * {
        INIT_METHOD(SegTree, "SegTree");

        CHECK_NOKWARGS("SegTree");

        if (args_data.size() != 2) {
            THROW_ARGUMENT_ERROR("SegTree.@new", "length",
                                 "2 arguments required");

            return nullptr;
        }

        SegTree *result = nullptr;

        // Construct from iterable
        auto iter = args_data[0]->iter();

        if (!iter) {
            THROW_ARGUMENT_ERROR("SegTree.@new", "iterable",
                                 "Requires an iterable object");

            return nullptr;
        }

        auto len = args_data[0]->len();

        if (!len) {
            THROW_ARGUMENT_ERROR("SegTree.@new", "iterable",
                                 "Requires an iterable with length");

            return nullptr;
        }

        int_t length = reinterpret_cast<Int *>(len)->data;

        auto vec = SegTree::New(vec_t(length * 2), args_data[1]);

        if (!vec) {
            return nullptr;
        }

        // To avoid it being freed
        Program::instance->tmp_stack.push_back(vec);
        auto &collect = vec->data;

        for (int i = 0; i < length; ++i) {
            Object *obj = iter->next();

            if (!obj) {
                goto error;
            }

            if (obj == enditer) {
                throw_fmt(
                    RuntimeError,
                    "SegTree.@new : Invalid length returned by iterable.@len");

                goto error;
            }

            collect[length + i] = obj;
        }

        // TODO A : Process segtree (only leaves set)

        result = vec;
        Program::instance->tmp_stack.pop_back();

        return result;

    error:
        Program::instance->tmp_stack.pop_back();

        return nullptr;
    };

    class_type->fn_traverse_objects = [](Object *self,
                                         const fn_visit_object_t &visit) {
        // Override and call super
        DynamicType::default_traverse_objects(self, visit);

        if (on_error())
            return;

        SegTree *me = reinterpret_cast<SegTree *>(self);

        for (auto o : me->data) {
            visit(o);
        }

        visit(me->functor);
    };

    // TODO A
    // @hash
    class_type->fn_hash = [](Object *self) -> Object * {
        auto me = reinterpret_cast<SegTree *>(self);

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
        auto me = reinterpret_cast<SegTree *>(self);

        auto result = new (nothrow) Int(me->data.size() / 2);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // TODO A
    // // @copy
    // class_type->fn_copy = [](Object *self) -> Object * {
    //     auto me = reinterpret_cast<SegTree *>(self);

    //     auto result = SegTree::New(me->data);

    //     if (!result) {
    //         return nullptr;
    //     }

    //     return result;
    // };

    // TODO A (only leaves)
    // class_type->fn_getitem = [](Object *self, Object *key) -> Object * {
    //     auto me = reinterpret_cast<SegTree *>(self);

    //     if (key->type == Int::class_type) {
    //         auto len = me->data.size();
    //         auto idx = get_mod_index(reinterpret_cast<Int *>(key)->data, len);

    //         // Out of bounds
    //         if (idx < 0 || idx >= len) {
    //             THROW_OUT_OF_BOUNDS(me->data.size(), idx);

    //             return nullptr;
    //         }

    //         auto result = me->data[idx];

    //         return result;
    //     } else {
    //         // Slice
    //         // Collect indices
    //         auto collect = try_collect_int_iterator(key, -me->data.size(),
    //                                                 me->data.size());

    //         if (on_error()) {
    //             clear_error();

    //             // Rethrow another one
    //             throw_fmt(
    //                 TypeError,
    //                 "Invalid type '%s' to index SegTree (must be Int or an "
    //                 "iterable)",
    //                 key->type->name.c_str());

    //             return nullptr;
    //         }

    //         vector<Object *> result;
    //         for (auto i : collect) {
    //             result.push_back(me->data[get_mod_index(i, me->data.size())]);
    //         }

    //         auto ret = new (nothrow) SegTree(result);

    //         if (!ret) {
    //             THROW_MEMORY_ERROR;

    //             return nullptr;
    //         }

    //         return ret;
    //     }
    // };

    // @str
    class_type->fn_str = [](Object *self) -> Object * {
        auto me = reinterpret_cast<SegTree *>(self);

        string result;

        if (printed_collections.find(me) != printed_collections.end()) {
            // Already printed
            result = "[...]";
        } else if (me->data.size() == 0) {
            result = "[]";
        } else {
            // To avoid infinite loops
            printed_collections.insert(me);

            result = "[";
            bool isfirst = true;
            for (int i = 0; i < me->data.size() / 2; ++i) {
                auto item = me->data[me->data.size() / 2 + i];
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
                    THROW_TYPE_ERROR_PREF("SegTree.@str", item_str->type,
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

    // TODO A : Update
    // // @setitem
    // class_type->fn_setitem = [](Object *self, Object *key,
    //                             Object *value) -> Object * {
    //     auto me = reinterpret_cast<SegTree *>(self);

    //     if (key->type == Int::class_type) {
    //         auto idx = get_mod_index(reinterpret_cast<Int *>(key)->data,
    //                                  me->data.size());

    //         // Out of bounds
    //         if (idx < 0 || idx >= me->data.size()) {
    //             THROW_OUT_OF_BOUNDS(me->data.size(), idx);

    //             return nullptr;
    //         }

    //         me->data[idx] = value;

    //         return null;
    //     } else {
    //         // Slice
    //         // Collect indices
    //         auto collect = try_collect_int_iterator(key, -me->data.size(),
    //                                                 me->data.size());

    //         if (on_error()) {
    //             clear_error();

    //             // Rethrow another one
    //             throw_fmt(
    //                 TypeError,
    //                 "Invalid type '%s' to index SegTree (must be Int or an "
    //                 "iterable)",
    //                 key->type->name.c_str());

    //             return nullptr;
    //         }

    //         // Map modular index to use only positive indinces
    //         for (auto &i : collect) {
    //             i = get_mod_index(i, me->data.size());
    //         }

    //         // Verify |step| is 1
    //         for (int i = 1; i < collect.size(); ++i) {
    //             int prev = collect[i - 1];
    //             int current = collect[i];

    //             if (abs(prev - current) != 1) {
    //                 throw_fmt(
    //                     IndexError,
    //                     "Invalid slice for %sSegTree.@setitem%s, the step "
    //                     "size must be %s1%s or %s-1%s",
    //                     C_GREEN, C_NORMAL, C_BLUE, C_NORMAL, C_BLUE, C_NORMAL);

    //                 return nullptr;
    //             }
    //         }

    //         const auto &[mn_it, mx_it] =
    //             minmax_element(collect.begin(), collect.end());

    //         if (mn_it == collect.end() || mx_it == collect.end()) {
    //             // Rethrow another one
    //             throw_fmt(
    //                 TypeError,
    //                 "Invalid type '%s' to index SegTree (must be Int or an "
    //                 "iterable)",
    //                 key->type->name.c_str());

    //             return nullptr;
    //         }

    //         int_t mn = *mn_it;
    //         int_t mx = *mx_it;

    //         str_t val;

    //         vector<Object *> newdata;

    //         for (int i = 0; i < mn; ++i)
    //             newdata.push_back(me->data[i]);

    //         newdata.push_back(value);

    //         for (int i = mx + 1; i < me->data.size(); ++i)
    //             newdata.push_back(me->data[i]);

    //         me->data = newdata;

    //         return null;
    //     }
    // };
}

void SegTree::init_class_objects() {
    class_hash = std::hash<str_t>()("SegTree");
}
