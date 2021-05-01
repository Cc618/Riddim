#include "usertype.hh"
#include "error.hh"
#include "map.hh"
#include "str.hh"

using namespace std;

typedef AttrObject UserObject;

Type *NewUserType(const str_t &name) {
    auto type = new (nothrow) Type(name);

    if (!type) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    type->constructor = [](Object *self, Object *args,
                                 Object *kwargs) -> Object * {
        // INIT_METHOD(HashMap, "HashMap");

        // CHECK_NOARGS("HashMap");
        // CHECK_NOKWARGS("HashMap");

        auto me = reinterpret_cast<Type*>(self);

        // TODO A
        auto result = NewUserObject(me);

        // Dispatch error
        if (!result)
            return nullptr;

        return result;
    };

    type->fn_getattr = UserObject::class_type->fn_getattr;
    type->fn_setattr = UserObject::class_type->fn_setattr;
    type->fn_str = UserObject::class_type->fn_str;

    // // @str
    // type->fn_str = [](Object *self) -> Object * {
    //     auto me = reinterpret_cast<Type *>(self);

    //     str_t result = me->name + "(";

    //     auto attrs = me->data->str();

    //     // Don't display attributes
    //     if (!attrs || attrs->type != Str::class_type) {
    //         clear_error();
    //         result += "{}";
    //     } else
    //         result += reinterpret_cast<Str *>(attrs)->data;

    //     auto result_str = new (nothrow) Str(result + ")");

    //     if (!result_str) {
    //         THROW_MEMORY_ERROR;

    //         return nullptr;
    //     }

    //     return result_str;
    // };


    return type;
}

Object *NewUserObject(Type *type) {
    auto me = AttrObject::New();

    if (!me) {
        return nullptr;
    }

    me->type = type;

    return me;
}
