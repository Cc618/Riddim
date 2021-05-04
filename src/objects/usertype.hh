#pragma once

// Types declared with newtype

#include "object.hh"
#include "map.hh"

typedef AttrObject UserObject;

UserTypeType *NewUserType(const str_t &name);

// The type must be a user defined type
UserObject *NewUserObject(Type *type);

// A subclass of type that handles user declared methods
// UserObject -> UserTypeType -> Type
struct UserTypeType : public Type {
    static Type *class_type;

    // "Class" attributes and methods
    HashMap *attrs;

    static UserTypeType *New(const str_t &name);

    static void init_class_type();

private:
    UserTypeType(const str_t &name);
};
