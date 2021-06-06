#include "set.hh"
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
#include <string>

using namespace std;

// --- HashSet ---
DynamicType *HashSet::class_type = nullptr;

size_t HashSet::class_hash = 0;

HashSet *HashSet::New(HashMap *data, bool create_data) {
    auto self = new (nothrow) HashSet(data);

    if (!self) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    if (create_data && data == nullptr) {
        self->data = HashMap::New();

        if (!self->data) {
            return nullptr;
        }
    }

    DynamicObject::init(self);

    if (on_error())
        return nullptr;

    return self;
}

HashSet::HashSet(HashMap *data)
    : DynamicObject(HashSet::class_type), data(data) {}

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
        } else if (args_data.size() == 1) {
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

                result->data->set(obj, null);

                if (on_error()) {
                    result = nullptr;
                    goto tmp_stack_unlock;
                }

                ++i;
            }

        tmp_stack_unlock:;
            Program::instance->tmp_stack.resize(old_tmp_stack_size);
        } else {
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

    // @cmp
    class_type->fn_cmp = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<HashSet *>(self);

        if (o->type != HashSet::class_type) {
            auto result = new (nothrow) Int(-1);

            if (!result) {
                THROW_MEMORY_ERROR;

                return nullptr;
            }

            return result;
        } else {
            return me->data->cmp(reinterpret_cast<HashSet *>(o)->data);
        }
    };

    // @copy
    class_type->fn_copy = [](Object *self) -> Object * {
        auto me = reinterpret_cast<HashSet *>(self);

        auto newdata = me->data->copy();

        if (!newdata) {
            return nullptr;
        }

        auto result = HashSet::New(reinterpret_cast<HashMap *>(newdata), false);

        if (!result) {
            return nullptr;
        }

        return result;
    };

    // @hash
    class_type->fn_hash = [](Object *self) -> Object * {
        auto me = reinterpret_cast<HashSet *>(self);

        auto result = reinterpret_cast<Int *>(me->data->hash());

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        // Combine the set hash
        result->data = hash_combine(class_hash, result->data);

        return result;
    };

    // @in
    class_type->fn_in = [](Object *self, Object *key) -> Object * {
        auto me = reinterpret_cast<HashSet *>(self);

        return me->data->in(key);
    };

    // @iter
    class_type->fn_iter = [](Object *self) -> Object * {
        auto me = reinterpret_cast<HashSet *>(self);

        auto internal_it = new hmap_t::iterator(me->data->data.begin());

        auto iter = new (nothrow) Iterator(
            [](Iterator *it) -> Object * {
                auto &internal_it =
                    *reinterpret_cast<hmap_t::iterator *>(it->custom_data);
                HashSet *me = reinterpret_cast<HashSet *>(it->collection);

                if (internal_it == me->data->data.end())
                    return enditer;

                auto result = internal_it->second.first;

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

    // @len
    class_type->fn_len = [](Object *self) -> Object * {
        auto me = reinterpret_cast<HashSet *>(self);

        return me->data->len();
    };

    // @str
    class_type->fn_str = [](Object *self) -> Object * {
        auto me = reinterpret_cast<HashSet *>(self);

        string result;

        if (printed_collections.find(me) != printed_collections.end()) {
            // Already printed
            result = "{...}";
        } else if (me->data->data.empty()) {
            result = "{}";
        } else {
            // To avoid infinite loops
            printed_collections.insert(me);

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
    INIT_METHOD(HashSet, "pop");

    CHECK_ARGSLEN(1, "HashSet.pop");
    CHECK_NOKWARGS("HashSet.pop");

    if (!me->data->me_pop_handler(me->data, args, kwargs)) {
        return nullptr;
    }

    return null;
}

// --- TreeSet ---
DynamicType *TreeSet::class_type = nullptr;

size_t TreeSet::class_hash = 0;

TreeSet *TreeSet::New(TreeMap *data, bool create_data) {
    auto self = new (nothrow) TreeSet(data);

    if (!self) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    if (create_data && data == nullptr) {
        self->data = TreeMap::New();

        if (!self->data) {
            return nullptr;
        }
    }

    DynamicObject::init(self);

    if (on_error())
        return nullptr;

    return self;
}

TreeSet::TreeSet(TreeMap *data)
    : DynamicObject(TreeSet::class_type), data(data) {}

void TreeSet::init_class_type() {
    class_type = DynamicType::New("TreeSet");

    if (!class_type) {
        return;
    }

    class_type->constructor = [](Object *self, Object *args,
                                 Object *kwargs) -> Object * {
        INIT_METHOD(TreeSet, "TreeSet");

        CHECK_NOKWARGS("TreeSet");

        TreeSet *result = nullptr;
        if (args_data.empty()) {
            result = TreeSet::New();

            // Dispatch error
            if (!result)
                return nullptr;
        } else if (args_data.size() == 1) {
            result = TreeSet::New();

            // Dispatch error
            if (!result)
                return nullptr;

            // Construct from iterable
            auto iter = args_data[0]->iter();

            if (!iter) {
                THROW_ARGUMENT_ERROR("TreeSet.@new", "iterable",
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

                result->data->data[obj] = null;

                if (on_error()) {
                    result = nullptr;
                    goto tmp_stack_unlock;
                }

                ++i;
            }

        tmp_stack_unlock:;
            Program::instance->tmp_stack.resize(old_tmp_stack_size);
        } else {
            THROW_ARGUMENT_ERROR("TreeSet.@new", "length",
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

        TreeSet *me = reinterpret_cast<TreeSet *>(self);

        visit(me->data);
    };

    // @cmp
    class_type->fn_cmp = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<TreeSet *>(self);

        if (o->type != TreeSet::class_type) {
            auto result = new (nothrow) Int(-1);

            if (!result) {
                THROW_MEMORY_ERROR;

                return nullptr;
            }

            return result;
        } else {
            return me->data->cmp(reinterpret_cast<TreeSet *>(o)->data);
        }
    };

    // @copy
    class_type->fn_copy = [](Object *self) -> Object * {
        auto me = reinterpret_cast<TreeSet *>(self);

        auto newdata = me->data->copy();

        if (!newdata) {
            return nullptr;
        }

        auto result = TreeSet::New(reinterpret_cast<TreeMap *>(newdata), false);

        if (!result) {
            return nullptr;
        }

        return result;
    };

    // @hash
    class_type->fn_hash = [](Object *self) -> Object * {
        auto me = reinterpret_cast<TreeSet *>(self);

        auto result = reinterpret_cast<Int *>(me->data->hash());

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        // Combine the set hash
        result->data = hash_combine(class_hash, result->data);

        return result;
    };

    // @in
    class_type->fn_in = [](Object *self, Object *key) -> Object * {
        auto me = reinterpret_cast<TreeSet *>(self);

        return me->data->in(key);
    };

    // @iter
    class_type->fn_iter = [](Object *self) -> Object * {
        auto me = reinterpret_cast<TreeSet *>(self);

        auto internal_it = new tmap_t::iterator(me->data->data.begin());

        auto iter = new (nothrow) Iterator(
            [](Iterator *it) -> Object * {
                auto &internal_it =
                    *reinterpret_cast<tmap_t::iterator *>(it->custom_data);
                TreeSet *me = reinterpret_cast<TreeSet *>(it->collection);

                if (internal_it == me->data->data.end())
                    return enditer;

                auto result = internal_it->first;

                if (!result) {
                    return nullptr;
                }

                ++internal_it;

                return result;
            },
            me, internal_it,
            [](Iterator *it) {
                delete reinterpret_cast<tmap_t::iterator *>(it->custom_data);
            });

        if (!iter) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return iter;
    };

    // @len
    class_type->fn_len = [](Object *self) -> Object * {
        auto me = reinterpret_cast<TreeSet *>(self);

        return me->data->len();
    };

    // @str
    class_type->fn_str = [](Object *self) -> Object * {
        auto me = reinterpret_cast<TreeSet *>(self);

        string result;

        if (printed_collections.find(me) != printed_collections.end()) {
            // Already printed
            result = "{...}";
        } else if (me->data->data.empty()) {
            result = "{}";
        } else {
            // To avoid infinite loops
            printed_collections.insert(me);

            result = "{";
            bool isfirst = true;
            for (const auto &[k, v] : me->data->data) {
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
}

void TreeSet::init_class_objects() {
    class_hash = std::hash<str_t>()("TreeSet");

    NEW_METHOD(TreeSet, add);
    method_add->doc_str = "Inserts an item";
    method_add->doc_signature = {{"item", false}};

    NEW_METHOD(TreeSet, pop);
    method_pop->doc_str = "Removes an item\n\n"
                          "- item : Which item to pop\n"
                          "- return : The popped item(s)";
    method_pop->doc_signature = {{"item", false}};
}

Object *TreeSet::me_add_handler(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(TreeSet, "add");

    CHECK_ARGSLEN(1, "TreeSet.add");
    CHECK_NOKWARGS("TreeSet.add");

    me->data->setitem(args_data[0], null);

    return null;
}

Object *TreeSet::me_pop_handler(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(TreeSet, "pop");

    CHECK_ARGSLEN(1, "TreeSet.pop");
    CHECK_NOKWARGS("TreeSet.pop");

    if (!me->data->me_pop_handler(me->data, args, kwargs)) {
        return nullptr;
    }

    return null;
}
