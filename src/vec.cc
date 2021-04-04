#include "vec.hh"
#include "bool.hh"
#include "error.hh"
#include "int.hh"
#include "null.hh"
#include "str.hh"
#include <algorithm>

using namespace std;

Type *Vec::class_type = nullptr;

Vec::Vec(const vec_t &data) : Object(Vec::class_type), data(data) {}

void Vec::init_class_type() {
    class_type = new (nothrow) Type("Vec");
    if (!class_type) {
        THROW_MEMORY_ERROR;
        return;
    }

    class_type->fn_traverse_objects = [](Object *self,
                                         const fn_visit_object_t &visit) {
        Vec *me = reinterpret_cast<Vec *>(self);

        for (auto o : me->data) {
            visit(o);
        }
    };

    // TODO : Hash

    // @copy
    class_type->fn_copy = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Vec *>(self);

        auto result = new (nothrow) Vec(me->data);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    class_type->fn_in = [](Object *self, Object *val) -> Object * {
        auto me = reinterpret_cast<Vec *>(self);

        // TODO : Override equal operator
        bool result =
            find_if(me->data.begin(), me->data.end(), [val](Object *o) -> bool {
                // TODO : Unsafe (no error handling), rm (see todo above)
                return reinterpret_cast<Int *>(o->hash())->data ==
                       reinterpret_cast<Int *>(val->hash())->data;
            }) != me->data.end();

        auto ret = new (nothrow) Bool(result);

        if (!ret) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return ret;
    };

    class_type->fn_getitem = [](Object *self, Object *key) -> Object * {
        auto me = reinterpret_cast<Vec *>(self);

        // TODO : Slice
        if (key->type != Int::class_type) {
            THROW_TYPE_ERROR_PREF("Vec.@getitem", key->type, Int::class_type);

            return nullptr;
        }

        auto idx = reinterpret_cast<Int *>(key)->data;

        // Out of bounds
        if (idx < 0 || idx >= me->data.size()) {
            THROW_OUT_OF_BOUNDS(me->data.size(), idx);

            return nullptr;
        }

        auto result = me->data[idx];

        return result;
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

        auto result_str = new (nothrow) Str(result);

        if (!result_str) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result_str;
    };

    class_type->fn_setitem = [](Object *self, Object *key,
                                Object *value) -> Object * {
        auto me = reinterpret_cast<Vec *>(self);

        // TODO : Slice
        if (key->type != Int::class_type) {
            THROW_TYPE_ERROR_PREF("Vec.@setitem", key->type, Int::class_type);

            return nullptr;
        }

        auto idx = reinterpret_cast<Int *>(key)->data;

        // Out of bounds
        if (idx < 0 || idx >= me->data.size()) {
            THROW_OUT_OF_BOUNDS(me->data.size(), idx);

            return nullptr;
        }

        me->data[idx] = value;

        return null;
    };
}
