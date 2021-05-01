#pragma once

// Types declared with newtype

#include "object.hh"

Type *NewUserType(const str_t &name);

// The type must be a user defined type
Object *NewUserObject(Type *type);
