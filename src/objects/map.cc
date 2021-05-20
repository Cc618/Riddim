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

            Object *obj = nullptr;
            int i = 0;
            while (1) {
                obj = iter->next();

                if (!obj) {
                    return nullptr;
                }

                if (obj == enditer)
                    break;

                auto len = obj->len();

                if (!len) {
                    return nullptr;
                }

                if (reinterpret_cast<Int *>(len)->data != 2) {
                    THROW_ARGUMENT_ERROR("HashMap.@new",
                                         "iterable[" + to_string(i) + "]",
                                         "Must contain 2 items");

                    return nullptr;
                }

                auto k = obj->getitem(Int::zero);

                if (!k) {
                    return nullptr;
                }

                auto v = obj->getitem(Int::one);

                if (!v) {
                    return nullptr;
                }

                result->set(k, v);
                ++i;
            }
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
