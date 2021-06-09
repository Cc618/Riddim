#include "deque.hh"
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

DynamicType *Deque::class_type = nullptr;

Deque *Deque::empty = nullptr;

size_t Deque::class_hash = 0;

Deque *Deque::New(const deque_t &data) {
    auto self = new (nothrow) Deque(data);

    if (!self) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    DynamicObject::init(self);

    if (on_error())
        return nullptr;

    return self;
}

Deque::Deque(const deque_t &data) : DynamicObject(Deque::class_type), data(data) {}

void Deque::init_class_type() {
    class_type = DynamicType::New("Deque");

    if (!class_type) {
        return;
    }

    class_type->constructor = [](Object *self, Object *args,
                                 Object *kwargs) -> Object * {
        INIT_METHOD(Deque, "Deque");

        CHECK_NOKWARGS("Deque");

        auto old_tmp_stack_size = Program::instance->tmp_stack.size();

        Deque *result = Deque::New();
        Program::instance->tmp_stack.push_back(result);

        // Dispatch error
        if (!result) {
            goto error;
        }

        if (args_data.size() == 1) {
            // Construct from iterable
            auto iter = args_data[0]->iter();

            if (!iter) {
                THROW_ARGUMENT_ERROR("Deque.@new", "iterable",
                                     "Requires an iterable object");

                goto error;
            }

            // To avoid it being freed
            auto &collect = result->data;

            Object *obj = nullptr;
            while (1) {
                obj = iter->next();

                if (!obj) {
                    goto error;
                }

                if (obj == enditer)
                    break;

                collect.push_back(obj);
            }
        } else if (!args_data.empty()) {
            THROW_ARGUMENT_ERROR("Deque.@new", "length",
                                 "0 or 1 arguments required");

            goto error;
        }

        Program::instance->tmp_stack.pop_back();

        return result;

    error:;
        Program::instance->tmp_stack.pop_back();

        return nullptr;
    };

    class_type->fn_traverse_objects = [](Object *self,
                                         const fn_visit_object_t &visit) {
        // Override and call super
        DynamicType::default_traverse_objects(self, visit);

        if (on_error())
            return;

        Deque *me = reinterpret_cast<Deque *>(self);

        for (auto o : me->data) {
            visit(o);
        }
    };

    // @add
    class_type->fn_add = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<Deque *>(self);

        if (o->type != Deque::class_type) {
            THROW_TYPE_ERROR_PREF("Deque.@add", o->type, Deque::class_type);

            return nullptr;
        }

        // PODs are copied
        auto odata = reinterpret_cast<Deque *>(o)->data;
        deque_t newdata(me->data.size() + odata.size());

        for (int i = 0; i < me->data.size(); ++i) {
            auto item = me->data[i];

            newdata[i] = is_pod_object(item) ? item->copy() : item;
        }

        for (int i = 0; i < odata.size(); ++i) {
            auto item = odata[i];

            newdata[me->data.size() + i] =
                is_pod_object(item) ? item->copy() : item;
        }

        auto result = new (nothrow) Deque(newdata);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @cmp
    class_type->fn_cmp = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<Deque *>(self);

        int_t res = 0;

        if (o->type != Deque::class_type) {
            res = -1;
        } else {
            // Algorithm :
            // - If not the same size, return the size difference
            // - Return the difference between the first different
            // item pair (where me[i] != other[i]) or 0 if none
            auto other = reinterpret_cast<Deque *>(o)->data;

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
        auto me = reinterpret_cast<Deque *>(self);

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
        auto me = reinterpret_cast<Deque *>(self);

        auto result = new (nothrow) Int(me->data.size());

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @copy
    class_type->fn_copy = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Deque *>(self);

        auto result = Deque::New(me->data);

        if (!result) {
            return nullptr;
        }

        return result;
    };

    // @in
    class_type->fn_in = [](Object *self, Object *val) -> Object * {
        auto me = reinterpret_cast<Deque *>(self);

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
        auto me = reinterpret_cast<Deque *>(self);

        auto iter = new (nothrow) Iterator(
            [](Iterator *it) -> Object * {
                int_t &i = reinterpret_cast<int_t &>(it->custom_data);
                Deque *me = reinterpret_cast<Deque *>(it->collection);

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
        auto me = reinterpret_cast<Deque *>(self);

        if (o->type != Int::class_type) {
            THROW_TYPE_ERROR_PREF("Deque.@mul", o->type, Int::class_type);

            return nullptr;
        }

        int_t multiplier = reinterpret_cast<Int *>(o)->data;

        if (multiplier < 0) {
            THROW_ARITHMETIC_ERROR("*", "Product requires a positive number");

            return nullptr;
        }

        deque_t data(me->data.size() * multiplier);
        for (size_t i = 0; i < data.size(); ++i) {
            auto item = me->data[i % me->data.size()];

            data[i] = is_pod_object(item) ? item->copy() : item;
        }

        auto result = Deque::New(data);

        if (!result) {
            return nullptr;
        }

        return result;
    };

    class_type->fn_getitem = [](Object *self, Object *key) -> Object * {
        auto me = reinterpret_cast<Deque *>(self);

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
                          "Invalid type '%s' to index Deque (must be Int or an "
                          "iterable)",
                          key->type->name.c_str());

                return nullptr;
            }

            deque<Object *> result;
            for (auto i : collect) {
                result.push_back(me->data[get_mod_index(i, me->data.size())]);
            }

            auto ret = new (nothrow) Deque(result);

            if (!ret) {
                THROW_MEMORY_ERROR;

                return nullptr;
            }

            return ret;
        }
    };

    // @str
    class_type->fn_str = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Deque *>(self);

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
                    THROW_TYPE_ERROR_PREF("Deque.@str", item_str->type,
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
        auto me = reinterpret_cast<Deque *>(self);

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
                          "Invalid type '%s' to index Deque (must be Int or an "
                          "iterable)",
                          key->type->name.c_str());

                return nullptr;
            }

            // Map modular index to use only positive indices
            for (auto &i : collect) {
                i = get_mod_index(i, me->data.size());
            }

            // Verify |step| is 1
            for (int i = 1; i < collect.size(); ++i) {
                int prev = collect[i - 1];
                int current = collect[i];

                if (abs(prev - current) != 1) {
                    throw_fmt(IndexError,
                              "Invalid slice for %sDeque.@setitem%s, the step "
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
                          "Invalid type '%s' to index Deque (must be Int or an "
                          "iterable)",
                          key->type->name.c_str());

                return nullptr;
            }

            int_t mn = *mn_it;
            int_t mx = *mx_it;

            str_t val;

            deque<Object *> newdata;

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

void Deque::init_class_objects() {
    empty = Deque::New();

    if (!empty) {
        return;
    }

    Program::add_global(empty);

    class_hash = std::hash<str_t>()("Deque");

    NEW_METHOD(Deque, add);
    method_add->doc_str = "Pushes back an item";
    method_add->doc_signature = {{"item", false}};

    NEW_METHOD(Deque, add_front);
    method_add_front->doc_str = "Pushes front an item";
    method_add_front->doc_signature = {{"item", false}};

    NEW_METHOD(Deque, pop);
    method_pop->doc_str = "Removes an item at a target index (by default, "
                          "the last item is popped out)\n\n"
                          "- [index : -1], Int / Range : Where to pop\n"
                          "- return : The popped item(s)";
    method_pop->doc_signature = {{"index", true}};
}

// --- Methods ---
Object *Deque::me_add_handler(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(Deque, "add");

    CHECK_ARGSLEN(1, "Deque.add");
    CHECK_NOKWARGS("Deque.add");

    me->data.push_back(args_data[0]);

    return null;
}

Object *Deque::me_add_front_handler(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(Deque, "add_front");

    CHECK_ARGSLEN(1, "Deque.add_front");
    CHECK_NOKWARGS("Deque.add_front");

    me->data.push_front(args_data[0]);

    return null;
}

Object *Deque::me_pop_handler(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(Deque, "pop");

    CHECK_NOKWARGS("Deque.pop");

    if (args_data.empty()) {
        if (me->data.empty()) {
            throw_str(IndexError, "Can't pop empty collection");

            return nullptr;
        }

        auto result = me->data.back();

        me->data.pop_back();

        return result;
    } else {
        auto index = args_data[0];

        if (index->type == Range::class_type) {
            auto slice = reinterpret_cast<Range *>(index);

            deque_t result;
            auto start = get_mod_index(slice->start, me->data.size());
            auto end = get_mod_index(slice->end, me->data.size());
            auto step = slice->step;

            if (end < me->data.size()) {
                if (step > 0) {
                    step = -step;
                } else {
                    swap(start, end);
                }

                int_t i = end;

                if (!slice->inclusive) {
                    i += step;
                }

                // Remove from back to front
                for (; i >= 0; i += step) {
                    if (i < start) {
                        break;
                    }

                    result.push_back(me->data[i]);
                    me->data.erase(me->data.begin() + i);
                }
            }

            reverse(result.begin(), result.end());

            auto oresult = Deque::New(result);

            if (!oresult) {
                return nullptr;
            }

            return oresult;
        } else if (index->type == Int::class_type) {
            int_t i = get_mod_index(reinterpret_cast<Int *>(index)->data,
                                    me->data.size());

            auto result = me->data[i];

            me->data.erase(me->data.begin() + i);

            return result;
        } else {
            throw_fmt(
                TypeError,
                "Deque.pop{index} : Index must be either %sInt%s or %sRange%s",
                C_BLUE, C_NORMAL, C_BLUE, C_NORMAL);

            return nullptr;
        }
    }
}
