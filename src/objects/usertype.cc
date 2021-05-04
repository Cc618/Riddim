#include "usertype.hh"
#include "bool.hh"
#include "error.hh"
#include "function.hh"
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

        auto instance = NewUserObject(me);

        // Dispatch error
        if (!instance)
            return nullptr;

        // Add attributes and methods
        for (const auto &[h, kv] : me->attrs->data) {
            const auto &[k, v] = kv;

            auto newv = v->copy();

            if (!newv)
                return nullptr;

            // If function, bind self
            if (newv->type == Builtin::class_type ||
                newv->type == Function::class_type) {
                auto newv_fun = reinterpret_cast<AbstractFunction *>(newv);
                newv_fun->self = instance;
            }

            if (!instance->setattr(k, newv))
                return nullptr;
        }

        // Call constructor if present
        auto new_key = new (nothrow) Str("@new");
        auto has_new = instance->data->in(new_key);
        bool is_constructed = false;

        if (!has_new) {
            return nullptr;
        }

        // @new present
        if (has_new == istrue) {
            auto new_attr = instance->getattr(new_key);

            if (!new_attr) {
                return nullptr;
            }

            if (new_attr != null) {
                // Call the constructor
                auto result = new_attr->call(args, kwargs);

                if (!result) {
                    return nullptr;
                }

                if (result != null) {
                    THROW_TYPE_ERROR_PREF((me->name + ".@new").c_str(),
                                          result->type, Null::class_type);

                    return nullptr;
                }

                is_constructed = true;
            }
        }

        // No args if no constructor
        if (!is_constructed && (!args_data.empty() || !kwargs_data.empty())) {
            THROW_ARGUMENT_ERROR((me->name + ".@new").c_str(), "length",
                                 "No arguments required");

            return nullptr;
        }

        return instance;
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

    class_type->fn_traverse_objects = [](Object *self,
                                         const fn_visit_object_t &visit) {
        // Override and call super
        if (Type::class_type->fn_traverse_objects) {
            Type::class_type->fn_traverse_objects(self, visit);

            if (on_error())
                return;
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

        if (key->type != Str::class_type) {
            THROW_TYPE_ERROR_PREF("UserTypeType.@setattr{key}", key->type,
                                  Str::class_type);

            return nullptr;
        }

        auto keystr = reinterpret_cast<Str *>(key)->data;

        // TODO D : @str
        if (keystr == "str") {
            if (value->type != Function::class_type) {
                THROW_TYPE_ERROR_PREF("UserTypeType.@setattr{value}(@str)",
                                      value->type, Function::class_type);

                return nullptr;
            }

            // auto val = reinterpret_cast<Function*>(value);

            // me->fn_str = [](Object *self) -> Object* {
            //     auto fn
            // }
        }

        return me->attrs->setitem(key, value);
    };
}

UserTypeType::UserTypeType(const str_t &name) : Type(name) {
    // Override type
    type = UserTypeType::class_type;
}
