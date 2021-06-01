#include "set.hh"
#include "bool.hh"
#include "builtins.hh"
#include "function.hh"
#include "error.hh"
#include "hash.hh"
#include "int.hh"
#include "iterator.hh"
#include "methods.hh"
#include "null.hh"
#include "program.hh"
#include "str.hh"
#include <string>

using namespace std;

// --- HashSet ---
DynamicType *HashSet::class_type = nullptr;

size_t HashSet::class_hash = 0;

HashSet *HashSet::New() {
    auto self = new (nothrow) HashSet();

    if (!self) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    self->data = HashMap::New();

    if (!self->data) {
        return nullptr;
    }

    DynamicObject::init(self);

    if (on_error())
        return nullptr;

    return self;
}

HashSet::HashSet()
    : DynamicObject(HashSet::class_type) {}

void HashSet::init_class_type() {
    class_type = DynamicType::New("HashSet");

    if (!class_type) {
        return;
    }

    class_type->constructor = [](Object *self, Object *args,
                                 Object *kwargs) -> Object * {
        INIT_METHOD(HashSet, "HashSet");

        CHECK_NOKWARGS("HashSet");

        HashSet *result = nullptr;
        if (args_data.empty()) {
            result = HashSet::New();

            // Dispatch error
            if (!result)
                return nullptr;
        } /* TODO A : else if (args_data.size() == 1) {
            result = HashSet::New();

            // Dispatch error
            if (!result)
                return nullptr;

            // Construct from iterable
            auto iter = args_data[0]->iter();

            if (!iter) {
                THROW_ARGUMENT_ERROR("HashSet.@new", "iterable",
                                     "Requires an iterable object");

                return nullptr;
            }

            auto old_tmp_stack_size = Program::instance->tmp_stack.size();

            Program::instance->tmp_stack.push_back(result);
            Program::instance->tmp_stack.push_back(iter);

            Object *obj = nullptr;
            int i = 0;
            while (1) {
                obj = iter->next();

                if (!obj) {
                    result = nullptr;
                    goto tmp_stack_unlock;
                }

                if (obj == enditer)
                    break;

                auto len = obj->len();

                if (!len) {
                    result = nullptr;
                    goto tmp_stack_unlock;
                }

                if (reinterpret_cast<Int *>(len)->data != 2) {
                    THROW_ARGUMENT_ERROR("HashSet.@new",
                                         "iterable[" + to_string(i) + "]",
                                         "Must contain 2 items");

                    result = nullptr;
                    goto tmp_stack_unlock;
                }

                Program::instance->tmp_stack.push_back(obj);

                auto k = obj->getitem(Int::zero);

                if (!k) {
                    result = nullptr;
                    goto tmp_stack_unlock;
                }

                Program::instance->tmp_stack.push_back(k);

                auto v = obj->getitem(Int::one);

                if (!v) {
                    result = nullptr;
                    goto tmp_stack_unlock;
                }

                Program::instance->tmp_stack.push_back(v);

                result->set(k, v);

                if (on_error()) {
                    result = nullptr;
                    goto tmp_stack_unlock;
                }

                ++i;
            }

        tmp_stack_unlock:;
            Program::instance->tmp_stack.resize(old_tmp_stack_size);
        } */ else {
            THROW_ARGUMENT_ERROR("HashSet.@new", "length",
                                 "0 or 1 arguments required");
        }

        return result;
    };

    class_type->fn_traverse_objects = [](Object *self,
                                         const fn_visit_object_t &visit) {
        // Override and call super
        DynamicType::default_traverse_objects(self, visit);

        if (on_error())
            return;

        HashSet *me = reinterpret_cast<HashSet *>(self);

        visit(me->data);
    };

    /* TODO A
    // @cmp
    class_type->fn_cmp = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<HashSet *>(self);

        int_t res = 0;

        if (o->type != HashSet::class_type) {
            res = -1;
        } else {
            auto other = reinterpret_cast<HashSet *>(o)->data;

            if (me->data.size() != other.size()) {
                res = -1;
            } else {
                for (const auto &[h, kv] : me->data) {
                    auto &[mek, mev] = kv;

                    // Key not found, different
                    auto other_it = other.find(h);
                    if (other_it == other.end()) {
                        res = -1;
                        break;
                    }

                    auto &[otherh, otherkv] = *other_it;
                    auto &[otherk, otherv] = otherkv;

                    // Int type verified
                    auto cmp = mev->cmp(otherv);

                    // Error
                    if (!cmp) {
                        return nullptr;
                    }

                    if (cmp) {
                        res = reinterpret_cast<Int *>(cmp)->data;
                        break;
                    }

                    // Int type verified
                    cmp = mek->cmp(otherk);

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

    // @copy
    class_type->fn_copy = [](Object *self) -> Object * {
        auto me = reinterpret_cast<HashSet *>(self);

        auto result = HashSet::New(me->data);

        if (!result) {
            return nullptr;
        }

        return result;
    };

    // @hash
    class_type->fn_hash = [](Object *self) -> Object * {
        auto me = reinterpret_cast<HashSet *>(self);

        int_t h = class_hash;

        for (const auto &[kh, kv] : me->data) {
            const auto &[k, v] = kv;

            // Value hash
            auto item_hash = v->hash();
            if (!item_hash) {
                return nullptr;
            }

            if (item_hash->type != Int::class_type) {
                THROW_TYPE_ERROR_PREF(v->type->name + ".@hash", item_hash->type,
                                      Int::class_type);

                return nullptr;
            }

            // Combine key
            h = hash_combine(h, kh);

            // Combine value
            h = hash_combine(h, reinterpret_cast<Int *>(item_hash)->data);
        }

        auto result = new (nothrow) Int(h);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };
    */

    // @in
    class_type->fn_in = [](Object *self, Object *key) -> Object * {
        auto me = reinterpret_cast<HashSet *>(self);

        return me->data->in(key);
    };

    /* TODO A :
    // @iter
    class_type->fn_iter = [](Object *self) -> Object * {
        auto me = reinterpret_cast<HashSet *>(self);

        auto internal_it = new hmap_t::iterator(me->data.begin());

        auto iter = new (nothrow) Iterator(
            [](Iterator *it) -> Object * {
                auto &internal_it =
                    *reinterpret_cast<hmap_t::iterator *>(it->custom_data);
                HashSet *me = reinterpret_cast<HashSet *>(it->collection);

                if (internal_it == me->data.end())
                    return enditer;

                auto result = Vec::New(
                    {internal_it->second.first, internal_it->second.second});

                if (!result) {
                    return nullptr;
                }

                ++internal_it;

                return result;
            },
            me, internal_it,
            [](Iterator *it) {
                delete reinterpret_cast<hmap_t::iterator *>(it->custom_data);
            });

        if (!iter) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return iter;
    };

    // @getitem
    class_type->fn_getitem = [](Object *self, Object *key) -> Object * {
        auto me = reinterpret_cast<HashSet *>(self);

        auto result = me->get(key);

        if (!result)
            return nullptr;

        return result;
    };

    // @len
    class_type->fn_len = [](Object *self) -> Object * {
        auto me = reinterpret_cast<HashSet *>(self);

        auto result = new (nothrow) Int(me->data.size());

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };
    */

    // @str
    class_type->fn_str = [](Object *self) -> Object * {
        auto me = reinterpret_cast<HashSet *>(self);

        string result;

        if (me->data->data.empty())
            result = "{}";
        else {
            result = "{";
            bool isfirst = true;
            for (const auto &[h, kv] : me->data->data) {
                const auto &[k, v] = kv;

                if (isfirst)
                    isfirst = false;
                else
                    result += ", ";

                auto k_str = k->str();

                // Error
                if (!k_str)
                    return nullptr;

                // Check type
                if (k_str->type != Str::class_type) {
                    THROW_TYPE_ERROR_PREF("@str", k_str->type, Str::class_type);

                    return nullptr;
                }

                result += reinterpret_cast<Str *>(k_str)->data;
            }

            result += "}";
        }

        auto result_str = Str::New(result);

        if (!result_str) {
            return nullptr;
        }

        return result_str;
    };

    /* TODO A
    // @setitem
    class_type->fn_setitem = [](Object *self, Object *key,
                                Object *value) -> Object * {
        auto me = reinterpret_cast<HashSet *>(self);

        me->set(key, value);

        if (on_error())
            return nullptr;

        return null;
    };
    */
}

void HashSet::init_class_objects() {
    class_hash = std::hash<str_t>()("HashSet");

    NEW_METHOD(HashSet, add);
    method_add->doc_str = "Inserts an item";
    method_add->doc_signature = {{"item", false}};

    NEW_METHOD(HashSet, pop);
    method_pop->doc_str = "Removes an item\n\n"
                          "- item : Which item to pop\n"
                          "- return : The popped item(s)";
    method_pop->doc_signature = {{"item", false}};
}

Object *HashSet::me_add_handler(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(HashSet, "add");

    CHECK_ARGSLEN(1, "HashSet.add");
    CHECK_NOKWARGS("HashSet.add");

    me->data->setitem(args_data[0], null);

    return null;
}

Object *HashSet::me_pop_handler(Object *self, Object *args, Object *kwargs) {
    // TODO A
    return nullptr;
}

