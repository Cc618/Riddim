#include "map.hh"
#include "bool.hh"
#include "builtins.hh"
#include "error.hh"
#include "hash.hh"
#include "int.hh"
#include "iterator.hh"
#include "methods.hh"
#include "null.hh"
#include "program.hh"
#include "str.hh"
#include "function.hh"
#include <string>

using namespace std;

// --- HashMap ---
DynamicType *HashMap::class_type = nullptr;

HashMap *HashMap::empty = nullptr;

size_t HashMap::class_hash = 0;

HashMap *HashMap::New(const hmap_t &data) {
    auto self = new (nothrow) HashMap(data);

    if (!self) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    DynamicObject::init(self);

    if (on_error())
        return nullptr;

    return self;
}

HashMap::HashMap(const hmap_t &data)
    : DynamicObject(HashMap::class_type), data(data) {}

void HashMap::init_class_type() {
    class_type = DynamicType::New("HashMap");

    if (!class_type) {
        return;
    }

    class_type->constructor = [](Object *self, Object *args,
                                 Object *kwargs) -> Object * {
        INIT_METHOD(HashMap, "HashMap");

        CHECK_NOKWARGS("HashMap");

        HashMap *result = nullptr;
        if (args_data.empty()) {
            result = HashMap::New();

            // Dispatch error
            if (!result)
                return nullptr;
        } else if (args_data.size() == 1) {
            result = HashMap::New();

            // Dispatch error
            if (!result)
                return nullptr;

            // Construct from iterable
            auto iter = args_data[0]->iter();

            if (!iter) {
                THROW_ARGUMENT_ERROR("HashMap.@new", "iterable",
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
                    THROW_ARGUMENT_ERROR("HashMap.@new",
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
        } else {
            THROW_ARGUMENT_ERROR("HashMap.@new", "length",
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

        HashMap *me = reinterpret_cast<HashMap *>(self);

        for (const auto &[h, kv] : me->data) {
            const auto &[k, v] = kv;

            visit(k);
            visit(v);
        }
    };

    // @cmp
    class_type->fn_cmp = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<HashMap *>(self);

        int_t res = 0;

        if (o->type != HashMap::class_type) {
            res = -1;
        } else {
            auto other = reinterpret_cast<HashMap *>(o)->data;

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
        auto me = reinterpret_cast<HashMap *>(self);

        auto result = HashMap::New(me->data);

        if (!result) {
            return nullptr;
        }

        return result;
    };

    // @hash
    class_type->fn_hash = [](Object *self) -> Object * {
        auto me = reinterpret_cast<HashMap *>(self);

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

    // @in
    class_type->fn_in = [](Object *self, Object *key) -> Object * {
        auto me = reinterpret_cast<HashMap *>(self);

        auto result = me->find(key) != me->data.end();

        return result ? istrue : isfalse;
    };

    // @iter
    class_type->fn_iter = [](Object *self) -> Object * {
        auto me = reinterpret_cast<HashMap *>(self);

        auto internal_it = new hmap_t::iterator(me->data.begin());

        auto iter = new (nothrow) Iterator(
            [](Iterator *it) -> Object * {
                auto &internal_it =
                    *reinterpret_cast<hmap_t::iterator *>(it->custom_data);
                HashMap *me = reinterpret_cast<HashMap *>(it->collection);

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
        auto me = reinterpret_cast<HashMap *>(self);

        auto result = me->get(key);

        if (!result)
            return nullptr;

        return result;
    };

    // @len
    class_type->fn_len = [](Object *self) -> Object * {
        auto me = reinterpret_cast<HashMap *>(self);

        auto result = new (nothrow) Int(me->data.size());

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @str
    class_type->fn_str = [](Object *self) -> Object * {
        auto me = reinterpret_cast<HashMap *>(self);

        string result;

        if (me->data.size() == 0)
            result = "{}";
        else {
            result = "{";
            bool isfirst = true;
            for (const auto &[h, kv] : me->data) {
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

                auto v_str = v->str();

                // Error
                if (!v_str)
                    return nullptr;

                // Check type
                if (v_str->type != Str::class_type) {
                    THROW_TYPE_ERROR_PREF("@str", v_str->type, Str::class_type);

                    return nullptr;
                }

                result += reinterpret_cast<Str *>(k_str)->data + ": " +
                          reinterpret_cast<Str *>(v_str)->data;
            }

            result += "}";
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
        auto me = reinterpret_cast<HashMap *>(self);

        me->set(key, value);

        if (on_error())
            return nullptr;

        return null;
    };
}

void HashMap::init_class_objects() {
    empty = HashMap::New();

    if (!empty) {
        return;
    }

    Program::add_global(empty);

    class_hash = std::hash<str_t>()("HashMap");

    NEW_METHOD(HashMap, pop);
    method_pop->doc_str = "Removes a mapping by key\n\n"
                          "- key : Key associated to mapping to pop\n"
                          "- return : The popped item(s)";
    method_pop->doc_signature = {{"key", false}};
}

Object *HashMap::get(Object *key) {
    auto it = find(key);

    if (it == data.end()) {
        // Not found
        if (!on_error()) {
            // Fetch string representation of the key
            auto k_name = key->str();
            str_t k_name_str;

            if (!k_name || k_name->type != Str::class_type) {
                clear_error();

                k_name_str = key->type->name + "()";
            } else
                k_name_str = reinterpret_cast<Str *>(k_name)->data;

            throw_fmt(IndexError, "Key %s not found", k_name_str.c_str());
        }

        return nullptr;
    }

    return (*it).second.second;
}

void HashMap::set(Object *key, Object *value) {
    auto h = key->hash();

    // Error
    if (!h)
        return;

    auto typeh = key->type->hash();

    if (!typeh)
        return;

    size_t h_key = hash_sz(hash_combine(reinterpret_cast<Int *>(h)->data,
                                        reinterpret_cast<Int *>(typeh)->data));

    data[h_key] = {key, value};
}

hmap_t::iterator HashMap::find(Object *key) {
    auto h = key->hash();

    // Error
    if (!h)
        return data.end();

    auto typeh = key->type->hash();

    if (!typeh)
        return data.end();

    size_t h_key = hash_sz(hash_combine(reinterpret_cast<Int *>(h)->data,
                                        reinterpret_cast<Int *>(typeh)->data));

    auto it = data.begin();
    for (; it != data.end(); ++it) {
        if ((*it).first == h_key && (*it).second.first->type == key->type)
            return it;
    }

    return data.find(h_key);
}

Object *HashMap::me_pop_handler(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(HashMap, "pop");

    CHECK_ARGSLEN(1, "HashMap.pop");
    CHECK_NOKWARGS("HashMap.pop");

    auto it = me->find(args_data[0]);

    if (on_error()) {
        return nullptr;
    }

    if (it == me->data.end()) {
        throw_fmt(IndexError, "HashMap.pop : Key not found");

        return nullptr;
    }

    auto result = it->second.second;

    me->data.erase(it);

    return result;
}

// --- TreeMap ---
bool TreeMapCompare::operator()(Object *x, Object *y) const {
    if (on_error()) {
        return false;
    }

    auto threeway = compare(x, y, CmpOp::Lesser);

    // Error
    if (!threeway) {
        return false;
    }

    return threeway->data;
}

DynamicType *TreeMap::class_type = nullptr;

TreeMap *TreeMap::empty = nullptr;

size_t TreeMap::class_hash = 0;

TreeMap *TreeMap::New(const tmap_t &data) {
    auto self = new (nothrow) TreeMap(data);

    if (!self) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    DynamicObject::init(self);

    if (on_error())
        return nullptr;

    return self;
}

TreeMap::TreeMap(const tmap_t &data)
    : DynamicObject(TreeMap::class_type), data(data) {}

void TreeMap::init_class_type() {
    class_type = DynamicType::New("TreeMap");

    if (!class_type) {
        return;
    }

    class_type->constructor = [](Object *self, Object *args,
                                 Object *kwargs) -> Object * {
        INIT_METHOD(TreeMap, "TreeMap");

        CHECK_NOKWARGS("TreeMap");

        TreeMap *result = nullptr;
        if (args_data.empty()) {
            result = TreeMap::New();

            // Dispatch error
            if (!result)
                return nullptr;
        } else if (args_data.size() == 1) {
            result = TreeMap::New();

            // Dispatch error
            if (!result)
                return nullptr;

            // Construct from iterable
            auto iter = args_data[0]->iter();

            if (!iter) {
                THROW_ARGUMENT_ERROR("TreeMap.@new", "iterable",
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
                    THROW_ARGUMENT_ERROR("TreeMap.@new",
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

                result->data[k] = v;

                if (on_error()) {
                    result = nullptr;
                    goto tmp_stack_unlock;
                }

                ++i;
            }

        tmp_stack_unlock:;
            Program::instance->tmp_stack.resize(old_tmp_stack_size);
        } else {
            THROW_ARGUMENT_ERROR("TreeMap.@new", "length",
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

        TreeMap *me = reinterpret_cast<TreeMap *>(self);

        for (const auto &[k, v] : me->data) {
            visit(k);
            visit(v);
        }
    };

    // @cmp
    class_type->fn_cmp = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<TreeMap *>(self);

        int_t res = 0;

        if (o->type != TreeMap::class_type) {
            res = -1;
        } else {
            auto other = reinterpret_cast<TreeMap *>(o)->data;

            if (me->data.size() != other.size()) {
                res = -1;
            } else {
                for (const auto &[mek, mev] : me->data) {
                    // Key not found, different
                    auto other_it = other.find(mek);
                    if (other_it == other.end()) {
                        res = -1;
                        break;
                    }

                    auto &[otherk, otherv] = *other_it;

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
        auto me = reinterpret_cast<TreeMap *>(self);

        auto result = TreeMap::New(me->data);

        if (!result) {
            return nullptr;
        }

        return result;
    };

    // @hash
    class_type->fn_hash = [](Object *self) -> Object * {
        auto me = reinterpret_cast<TreeMap *>(self);

        int_t h = class_hash;

        for (const auto &[k, v] : me->data) {
            // Key hash
            auto key_hash = k->hash();
            if (!key_hash) {
                return nullptr;
            }

            if (key_hash->type != Int::class_type) {
                THROW_TYPE_ERROR_PREF(k->type->name + ".@hash", key_hash->type,
                                      Int::class_type);

                return nullptr;
            }

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
            h = hash_combine(h, reinterpret_cast<Int *>(key_hash)->data);

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

    // @in
    class_type->fn_in = [](Object *self, Object *key) -> Object * {
        auto me = reinterpret_cast<TreeMap *>(self);

        auto result = me->data.find(key) != me->data.end();

        if (on_error()) {
            return nullptr;
        }

        return result ? istrue : isfalse;
    };

    // @iter
    class_type->fn_iter = [](Object *self) -> Object * {
        auto me = reinterpret_cast<TreeMap *>(self);

        auto internal_it = new tmap_t::iterator(me->data.begin());

        auto iter = new (nothrow) Iterator(
            [](Iterator *it) -> Object * {
                auto &internal_it =
                    *reinterpret_cast<tmap_t::iterator *>(it->custom_data);
                TreeMap *me = reinterpret_cast<TreeMap *>(it->collection);

                if (internal_it == me->data.end())
                    return enditer;

                auto result =
                    Vec::New({internal_it->first, internal_it->second});

                if (!result) {
                    return nullptr;
                }

                ++internal_it;

                if (on_error()) {
                    return nullptr;
                }

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

    // @getitem
    class_type->fn_getitem = [](Object *self, Object *key) -> Object * {
        auto me = reinterpret_cast<TreeMap *>(self);

        auto it = me->data.find(key);

        // Found
        if (!on_error() && it != me->data.end()) {
            auto result = it->second;

            if (!result)
                return nullptr;

            return result;
        }

        // Not found
        if (!on_error()) {
            // Fetch string representation of the key
            auto k_name = key->str();
            str_t k_name_str;

            if (!k_name || k_name->type != Str::class_type) {
                clear_error();

                k_name_str = key->type->name + "()";
            } else
                k_name_str = reinterpret_cast<Str *>(k_name)->data;

            throw_fmt(IndexError, "Key %s not found", k_name_str.c_str());
        }

        return nullptr;
    };

    // @len
    class_type->fn_len = [](Object *self) -> Object * {
        auto me = reinterpret_cast<TreeMap *>(self);

        auto result = new (nothrow) Int(me->data.size());

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @str
    class_type->fn_str = [](Object *self) -> Object * {
        auto me = reinterpret_cast<TreeMap *>(self);

        string result;

        if (me->data.size() == 0)
            result = "{}";
        else {
            result = "{";
            bool isfirst = true;
            for (const auto &[k, v] : me->data) {
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

                auto v_str = v->str();

                // Error
                if (!v_str)
                    return nullptr;

                // Check type
                if (v_str->type != Str::class_type) {
                    THROW_TYPE_ERROR_PREF("@str", v_str->type, Str::class_type);

                    return nullptr;
                }

                result += reinterpret_cast<Str *>(k_str)->data + ": " +
                          reinterpret_cast<Str *>(v_str)->data;
            }

            result += "}";
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
        auto me = reinterpret_cast<TreeMap *>(self);

        auto it = me->data.find(key);

        // Do not add item if error
        if (on_error()) {
            return nullptr;
        }

        me->data[key] = value;

        // Should not be executed since the find method has not thrown
        if (on_error()) {
            return nullptr;
        }

        return null;
    };
}

void TreeMap::init_class_objects() {
    empty = TreeMap::New();

    if (!empty) {
        return;
    }

    Program::add_global(empty);

    class_hash = std::hash<str_t>()("TreeMap");
}

// --- AttrObject ---
Type *AttrObject::class_type = nullptr;

size_t AttrObject::class_hash = 0;

AttrObject *AttrObject::New() {
    auto o = new (nothrow) AttrObject();

    if (!o) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    o->data = HashMap::New();

    if (!o->data) {
        return nullptr;
    }

    return o;
}

void AttrObject::init_class_type() {
    class_type = new (nothrow) Type("AttrObject");
    if (!class_type) {
        THROW_MEMORY_ERROR;
        return;
    }

    class_type->constructor = [](Object *self, Object *args,
                                 Object *kwargs) -> Object * {
        INIT_METHOD(Object, "AttrObject");

        CHECK_NOARGS("AttrObject");
        CHECK_NOKWARGS("AttrObject");

        auto result = AttrObject::New();

        // Dispatch error
        if (!result)
            return nullptr;

        return result;
    };

    class_type->fn_traverse_objects = [](Object *self,
                                         const fn_visit_object_t &visit) {
        AttrObject *me = reinterpret_cast<AttrObject *>(self);

        visit(me->data);
    };

    class_type->fn_getattr = [](Object *self, Object *name) -> Object * {
        auto me = reinterpret_cast<AttrObject *>(self);

        auto result = me->data->getitem(name);

        // Change error to AttrError
        if (!result) {
            clear_error();

            auto name_str = name->str();

            if (!name_str || name_str->type != Str::class_type) {
                clear_error();
                throw_fmt(NameError, "No such attribute for type %s",
                          self->type);
            } else
                THROW_ATTR_ERROR(self->type,
                                 reinterpret_cast<Str *>(name_str)->data);

            return nullptr;
        }

        return result;
    };

    // @hash
    class_type->fn_hash = [](Object *self) -> Object * {
        auto me = reinterpret_cast<AttrObject *>(self);

        auto map_hash = me->data->hash();

        // Dispatch error
        if (!map_hash)
            return nullptr;

        int_t h = class_hash;

        // The type Int* is guaranted for builtin type hashes
        auto result = new (nothrow)
            Int(hash_combine(h, reinterpret_cast<Int *>(map_hash)->data));

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @copy
    class_type->fn_copy = [](Object *self) -> Object * {
        auto me = reinterpret_cast<AttrObject *>(self);

        auto data_copy = me->data->copy();

        if (!data_copy) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        if (data_copy->type != HashMap::class_type) {
            THROW_TYPE_ERROR_PREF("AttrObject.@copy", data_copy->type,
                                  HashMap::class_type);

            return nullptr;
        }

        auto result =
            new (nothrow) AttrObject(reinterpret_cast<HashMap *>(data_copy));

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @in
    class_type->fn_in = [](Object *self, Object *val) -> Object * {
        auto me = reinterpret_cast<AttrObject *>(self);

        return me->data->in(val);
    };

    // @len
    class_type->fn_len = [](Object *self) -> Object * {
        auto me = reinterpret_cast<AttrObject *>(self);

        return me->data->len();
    };

    // @str
    class_type->fn_str = [](Object *self) -> Object * {
        auto me = reinterpret_cast<AttrObject *>(self);

        // Since AttrObject is used for a user defined type
        // The type can derive from AttrObject and have a different name
        str_t result = me->type->name + "(";

        auto attrs = me->data->str();

        // Don't display attributes
        if (!attrs || attrs->type != Str::class_type) {
            clear_error();
            result += "{}";
        } else
            result += reinterpret_cast<Str *>(attrs)->data;

        auto result_str = Str::New(result + ")");

        if (!result_str) {
            return nullptr;
        }

        return result_str;
    };

    class_type->fn_setattr = [](Object *self, Object *name,
                                Object *value) -> Object * {
        auto me = reinterpret_cast<AttrObject *>(self);

        auto result = me->data->setitem(name, value);

        // Change error to AttrError
        if (!result) {
            clear_error();

            auto name_str = name->str();

            if (!name_str || name_str->type != Str::class_type) {
                clear_error();
                throw_fmt(NameError, "No such attribute for type %s",
                          self->type);
            } else
                THROW_ATTR_ERROR(self->type,
                                 reinterpret_cast<Str *>(name_str)->data);

            return nullptr;
        }

        return result;
    };
}

AttrObject::AttrObject(HashMap *data)
    : Object(AttrObject::class_type), data(data) {}

void AttrObject::init_class_objects() {
    class_hash = std::hash<str_t>()("AttrObject");
}
