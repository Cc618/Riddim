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

SegTree *SegTree::New(const vec_t &data, Object *functor, Object *init_val) {
    auto self = new (nothrow) SegTree(data, functor, init_val);

    if (!self) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    DynamicObject::init(self);

    if (on_error())
        return nullptr;

    return self;
}

SegTree::SegTree(const vec_t &data, Object *functor, Object *init_val)
    : DynamicObject(SegTree::class_type), data(data), functor(functor),
      init_val(init_val) {}

void SegTree::init_class_type() {
    class_type = DynamicType::New("SegTree");

    if (!class_type) {
        return;
    }

    class_type->constructor = [](Object *self, Object *args,
                                 Object *kwargs) -> Object * {
        INIT_METHOD(SegTree, "SegTree");

        CHECK_NOKWARGS("SegTree");

        if (args_data.size() != 2 && args_data.size() != 3) {
            THROW_ARGUMENT_ERROR("SegTree.@new", "length",
                                 "2 or 3 arguments required");

            return nullptr;
        }

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

        auto result =
            SegTree::New(vec_t(length * 2), args_data[1],
                         args_data.size() == 2 ? Int::zero : args_data[2]);

        if (!result) {
            return nullptr;
        }

        // To avoid it being freed
        Program::instance->tmp_stack.push_back(result);
        auto &collect = result->data;

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

        // Init the segment tree
        for (int_t i = length - 1; i > 0; --i) {
            auto functor_arg = Vec::New({result->data[i * 2], result->data[i * 2 + 1]});
            if (!functor_arg) {
                goto error;
            }

            auto functor_args = Vec::New({functor_arg});
            if (!functor_args) {
                goto error;
            }

            result->data[i] = result->functor->call(functor_args, HashMap::empty);

            if (!result->data[i]) {
                goto error;
            }
        }

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
        visit(me->init_val);
    };

    // @hash
    class_type->fn_hash = [](Object *self) -> Object * {
        auto me = reinterpret_cast<SegTree *>(self);

        int_t h = class_hash;

        for (int i = me->data.size() / 2; i < me->data.size(); ++i) {
            auto o = me->data[i];
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

    // @copy
    class_type->fn_copy = [](Object *self) -> Object * {
        auto me = reinterpret_cast<SegTree *>(self);

        auto result = SegTree::New(me->data, me->functor, me->init_val);

        if (!result) {
            return nullptr;
        }

        return result;
    };

    class_type->fn_getitem = [](Object *self, Object *key) -> Object * {
        auto me = reinterpret_cast<SegTree *>(self);

        auto len = me->data.size() / 2;

        if (key->type == Int::class_type) {
            auto idx = get_mod_index(reinterpret_cast<Int *>(key)->data, len);

            // Out of bounds
            if (idx < 0 || idx >= len) {
                THROW_OUT_OF_BOUNDS(len, idx);

                return nullptr;
            }

            auto result = me->data[len + idx];

            return result;
        } else {
            // Slice
            // Collect indices
            auto collect = try_collect_int_iterator(key, -len, len);

            if (on_error()) {
                clear_error();

                // Rethrow another one
                throw_fmt(
                    TypeError,
                    "Invalid type '%s' to index SegTree (must be Int or an "
                    "iterable)",
                    key->type->name.c_str());

                return nullptr;
            }

            vector<Object *> result;
            for (auto i : collect) {
                result.push_back(me->data[len + get_mod_index(i, len)]);
            }

            auto ret = Vec::New(result);

            if (!ret) {
                return nullptr;
            }

            return ret;
        }
    };

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

    // @setitem
    class_type->fn_setitem = [](Object *self, Object *key,
                                Object *value) -> Object * {
        auto me = reinterpret_cast<SegTree *>(self);

        if (key->type != Int::class_type) {
            THROW_TYPE_ERROR_PREF("SegTree.@setitem", key->type,
                                  Int::class_type);

            return nullptr;
        }

        auto idx =
            get_mod_index(reinterpret_cast<Int *>(key)->data, me->data.size());

        // Out of bounds
        if (idx < 0 || idx >= me->data.size()) {
            THROW_OUT_OF_BOUNDS(me->data.size(), idx);

            return nullptr;
        }

        idx += me->data.size() / 2;
        me->data[idx] = value;

        do {
            idx /= 2;

            auto functor_arg = Vec::New({me->data[idx * 2], me->data[idx * 2 + 1]});
            if (!functor_arg) {
                return nullptr;
            }

            auto functor_args = Vec::New({functor_arg});
            if (!functor_args) {
                return nullptr;
            }

            me->data[idx] = me->functor->call(functor_args, HashMap::empty);

            if (!me->data[idx]) {
                return nullptr;
            }
        } while (idx > 1);

        return null;
    };
}

void SegTree::init_class_objects() {
    class_hash = std::hash<str_t>()("SegTree");

    NEW_METHOD(SegTree, query);
    method_query->doc_str =
        "Queries the segment tree from start (included)"
        " to end (excluded)\n\n"
        "- start, Int : First index (included)\n"
        "- end, Int : Last index (excluded)\n"
        "- [init_val, Int] : The default value when the range is empty "
        "(initializes the query)\n"
        "- return : The application of the functor on this range\n\n"
        "* Note : It is equivalent to functor(data[start -> end]) "
        "but with log N time complexity";
    method_query->doc_signature = {
        {"start", false}, {"end", false}, {"init_val : 0", true}};
}

// --- Methods ---
Object *SegTree::me_query_handler(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(SegTree, "query");

    CHECK_NOKWARGS("SegTree.query");

    if (args_data.size() != 2) {
        THROW_ARGUMENT_ERROR("SegTree.query", "length", "2 arguments required");

        return nullptr;
    }

    if (args_data[0]->type != Int::class_type) {
        THROW_TYPE_ERROR_PREF("SegTree.query{start}", args_data[0]->type,
                              Int::class_type);

        return nullptr;
    }

    if (args_data[1]->type != Int::class_type) {
        THROW_TYPE_ERROR_PREF("SegTree.query{end}", args_data[1]->type,
                              Int::class_type);

        return nullptr;
    }

    auto start = reinterpret_cast<Int *>(args_data[0])->data;
    auto end = reinterpret_cast<Int *>(args_data[1])->data;

    const auto n = me->data.size() / 2;

    if (end > n) {
        THROW_OUT_OF_BOUNDS(n, end);

        return nullptr;
    }

    if (start < 0 || start > end) {
        THROW_OUT_OF_BOUNDS(0, start);

        return nullptr;
    }

    start += n;
    end += n;
    Object *result = me->init_val;

    auto old_tmp_stack_size = Program::instance->tmp_stack.size();

    while (start < end && start != 0) {
        if (start % 2 == 1) {
            auto functor_arg = Vec::New({result, me->data[start++]});
            if (!functor_arg) {
                goto error;
            }

            auto functor_args = Vec::New({functor_arg});
            if (!functor_args) {
                goto error;
            }

            result = me->functor->call(functor_args, HashMap::empty);

            Program::instance->tmp_stack.push_back(result);

            if (!result) {
                goto error;
            }
        }

        if (end % 2 == 1) {
            auto functor_arg = Vec::New({result, me->data[--end]});
            if (!functor_arg) {
                goto error;
            }

            auto functor_args = Vec::New({functor_arg});
            if (!functor_args) {
                goto error;
            }

            result = me->functor->call(functor_args, HashMap::empty);

            Program::instance->tmp_stack.push_back(result);

            if (!result) {
                goto error;
            }
        }

        start /= 2;
        end /= 2;
    }

    Program::instance->tmp_stack.resize(old_tmp_stack_size);

    return result;

error:
    Program::instance->tmp_stack.resize(old_tmp_stack_size);

    return nullptr;
}
