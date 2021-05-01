#include "usertype.hh"
#include "error.hh"
#include "methods.hh"
#include "null.hh"
#include "str.hh"
#include "vec.hh"

using namespace std;

UserTypeType *NewUserType(const str_t &name) {
    auto type = UserTypeType::New(name);

    if (!type) {
        return nullptr;
    }

    type->constructor = [name](Object *self, Object *args,
                               Object *kwargs) -> Object * {
        INIT_METHOD(UserTypeType, name);

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

UserObject *NewUserObject(Type *type) {
    auto me = UserObject::New();

    if (!me) {
        return nullptr;
    }

    me->type = type;

    return me;
}

UserTypeType *UserTypeType::New(const str_t &name) {
    auto me = new (nothrow) UserTypeType(name);

    if (!me) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    me->attrs = HashMap::New();

    if (!me->attrs) {
        return nullptr;
    }

    return me;
}

Type *UserTypeType::class_type = nullptr;

void UserTypeType::init_class_type() {
    class_type = new Type("UserTypeType");

    // Inherit from super type (Type)
    Type::init_slots(class_type);

    class_type->fn_traverse_objects =
        [](Object *self, const fn_visit_object_t &visit) {
        // Override and call super
        if (Type::class_type->fn_traverse_objects) {
            Type::class_type->fn_traverse_objects(self, visit);

            if (on_error()) return;
        }

        auto me = reinterpret_cast<UserTypeType *>(self);

        visit(me->attrs);
    };

    // @getattr
    class_type->fn_getattr = [](Object *self, Object *key) -> Object * {
        auto me = reinterpret_cast<UserTypeType *>(self);

        return me->attrs->getitem(key);
    };

    // @setattr
    class_type->fn_setattr = [](Object *self, Object *key,
                                Object *value) -> Object * {
        auto me = reinterpret_cast<UserTypeType *>(self);

        return me->attrs->setitem(key, value);
    };

    // @call
    // Call the constructor
    class_type->fn_call = [](Object *self, Object *args,
                             Object *kwargs) -> Object * {
        auto me = reinterpret_cast<UserTypeType *>(self);

        // TODO C : Init slots
        // TODO C : Custom constructor + no constructor
        if (!me->constructor) {
            throw_fmt(NameError, "Type %s has no constructor",
                      me->name.c_str());

            return nullptr;
        }


        return me->constructor(self, args, kwargs);
    };
}

UserTypeType::UserTypeType(const str_t &name) : Type(name) {
    // Override type
    type = UserTypeType::class_type;
}
