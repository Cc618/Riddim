#include "usertype.hh"
#include "error.hh"
#include "map.hh"
#include "str.hh"
#include "vec.hh"
#include "methods.hh"

using namespace std;

typedef AttrObject UserObject;

Type *NewUserType(const str_t &name) {
    auto type = new (nothrow) Type(name);

    if (!type) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    type->constructor = [name](Object *self, Object *args,
                                 Object *kwargs) -> Object * {
        INIT_METHOD(Type, name);

        auto result = NewUserObject(me);

        // Dispatch error
        if (!result)
            return nullptr;

        // TODO C : Custom constructor

        return result;
    };

    // Default slots
    type->fn_copy = UserObject::class_type->fn_copy;
    type->fn_getattr = UserObject::class_type->fn_getattr;
    type->fn_setattr = UserObject::class_type->fn_setattr;
    type->fn_str = UserObject::class_type->fn_str;

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
