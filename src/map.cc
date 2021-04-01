#include "map.hh"
#include "error.hh"
#include "hash.hh"
#include "int.hh"
#include "str.hh"

using namespace std;

Type *HashMap::class_type = nullptr;

HashMap::HashMap() : Object(HashMap::class_type) {}

void HashMap::init_class_type() {
    class_type = new (nothrow) Type("HashMap");
    if (!class_type) {
        THROW_MEMORY_ERROR;
        return;
    }

    class_type->fn_traverse_objects = [](Object *self,
                                         const fn_visit_object_t &visit) {
        HashMap *me = reinterpret_cast<HashMap *>(self);

        for (const auto &[h, kv] : me->data) {
            const auto &[k, v] = kv;

            visit(k);
            visit(v);
        }
    };

    // TODO : Hash
    // @str
    class_type->fn_str = [](Object *self) -> Object * {
        auto me = reinterpret_cast<HashMap *>(self);

        string result;

        if (me->data.size() == 0)
            result = "{}";
        else {
            result = "{ ";
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

            result += " }";
        }

        auto result_str = new (nothrow) Str(result);

        if (!result_str) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result_str;
    };

    class_type->fn_setitem = [](Object *self, Object *key, Object *value) -> Object * {
        auto me = reinterpret_cast<HashMap*>(self);

        me->setitem(key, value);

        if (on_error()) return nullptr;

        // TODO 1 : Return null
        return nullptr;
    };
}

Object *HashMap::getitem(Object *key) {
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

void HashMap::setitem(Object *key, Object *value) {
    auto h = key->hash();

    // Error
    if (!h)
        return;

    // Check type
    if (h->type != Int::class_type) {
        THROW_TYPE_ERROR_PREF("@hash", h->type, Int::class_type);

        return;
    }

    size_t h_key = hash_sz(reinterpret_cast<Int *>(h)->data);

    data[h_key] = {key, value};
}

hmap_t::iterator HashMap::find(Object *key) {
    auto h = key->hash();

    // Error
    if (!h)
        return data.end();

    // Check type
    if (h->type != Int::class_type) {
        THROW_TYPE_ERROR_PREF("@hash", h->type, Int::class_type);

        return data.end();
    }

    size_t h_key = hash_sz(reinterpret_cast<Int *>(h)->data);

    return data.find(h_key);
}
