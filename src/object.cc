#include "object.hh"
#include "error.hh"
#include "hash.hh"
#include "int.hh"
#include "program.hh"
#include "str.hh"
#include "bool.hh"
#include <iostream>

using namespace std;

bool is_pod_object(Object *o) {
    if (o->type == Int::class_type) return true;
    if (o->type == Bool::class_type) return true;

    return false;
}

// --- Object ---
Type *Object::class_type = nullptr;

void Object::init_class_type() { class_type = new Type("Object"); }

Object::Object(Type *type) : type(type) {
    init_gc_data(this);

    // TODO : Call new function (of the type) ?
}

Object::~Object() {
    // TODO : Call delete function (of the type) ?
}

void Object::traverse_objects(const fn_visit_object_t &visit) {
    if (type->fn_traverse_objects)
        type->fn_traverse_objects(this, visit);
}

Object *Object::add(Object *o) {
    if (!type->fn_add) {
        THROW_NOBUILTIN(this->type, add);

        return nullptr;
    }

    return type->fn_add(this, o);
}

Object *Object::call(Object *args, Object *kwargs) {
    if (!type->fn_call) {
        THROW_NOBUILTIN(this->type, call);

        return nullptr;
    }

    return type->fn_call(this, args, kwargs);
}

Object *Object::cmp(Object *o) {
    if (!type->fn_cmp) {
        bool iseq = this == o;

        auto result = new (nothrow) Int(iseq ? 0 : -1);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    }

    return type->fn_cmp(this, o);
}

Object *Object::copy() {
    if (!type->fn_copy) {
        return this;
    }

    return type->fn_copy(this);
}

Object *Object::div(Object *o) {
    if (!type->fn_div) {
        THROW_NOBUILTIN(this->type, div);

        return nullptr;
    }

    return type->fn_div(this, o);
}

Object *Object::getattr(Object *name) {
    if (!type->fn_getattr) {
        auto name_str = name->str();

        if (!name_str || name_str->type != Str::class_type) {
            clear_error();
            throw_fmt(NameError, "No such attribute for type %s", type);
        } else
            THROW_ATTR_ERROR(type, reinterpret_cast<Str *>(name_str)->data);

        return nullptr;
    }

    return type->fn_getattr(this, name);
}

Object *Object::getitem(Object *args) {
    if (!type->fn_getitem) {
        THROW_NOBUILTIN(this->type, getitem);

        return nullptr;
    }

    return type->fn_getitem(this, args);
}

Object *Object::hash() {
    // Default hash using the object memory address and its type
    if (!type->fn_hash) {
        auto result =
            new (nothrow) Int(hash_combine(hash_ptr(this), hash_ptr(type)));

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    }

    return type->fn_hash(this);
}

Object *Object::in(Object *val) {
    if (!type->fn_in) {
        THROW_NOBUILTIN(this->type, in);

        return nullptr;
    }

    return type->fn_in(this, val);
}

Object *Object::len() {
    if (!type->fn_len) {
        THROW_NOBUILTIN(this->type, len);

        return nullptr;
    }

    return type->fn_len(this);
}

Object *Object::mod(Object *o) {
    if (!type->fn_div) {
        THROW_NOBUILTIN(this->type, mod);

        return nullptr;
    }

    return type->fn_mod(this, o);
}

Object *Object::mul(Object *o) {
    if (!type->fn_mul) {
        THROW_NOBUILTIN(this->type, mul);

        return nullptr;
    }

    return type->fn_mul(this, o);
}

Object *Object::neg() {
    if (!type->fn_neg) {
        THROW_NOBUILTIN(this->type, neg);

        return nullptr;
    }

    return type->fn_neg(this);
}

Object *Object::setattr(Object *name, Object *value) {
    if (!type->fn_setattr) {
        auto name_str = name->str();

        if (!name_str || name_str->type != Str::class_type) {
            clear_error();
            throw_fmt(NameError, "No such attribute for type %s", type);
        } else
            THROW_ATTR_ERROR(type, reinterpret_cast<Str *>(name_str)->data);

        return nullptr;
    }

    return type->fn_setattr(this, name, value);
}

Object *Object::setitem(Object *key, Object *value) {
    if (!type->fn_setitem) {
        THROW_NOBUILTIN(this->type, setitem);

        return nullptr;
    }

    return type->fn_setitem(this, key, value);
}

Object *Object::str() {
    // TODO : Default, return Type()
    if (!type->fn_str) {
        auto result = new (nothrow) Str(type->name + "()");

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    }

    return type->fn_str(this);
}

Object *Object::sub(Object *o) {
    if (!type->fn_sub) {
        // a - b == a + (-b)
        if (type->fn_add && type->fn_neg) {
            auto negated = o->neg();

            if (!negated) return nullptr;

            auto result = add(negated);

            if (!result) return nullptr;

            return result;
        }

        THROW_NOBUILTIN(this->type, sub);

        return nullptr;
    }

    return type->fn_sub(this, o);
}

// --- Type ---
static int type_global_id = 0;

Type *Type::class_type = nullptr;

Type::Type(const str_t &name) : Object(Type::class_type), name(name) {
    // Register this type to the program
    if (Program::instance)
        Program::add_type(this);

    id = ++type_global_id;
}

void Type::init_class_type() {
    class_type = new Type("Type");

    // Was not initialized
    class_type->type = class_type;

    // @call
    // Call the constructor
    class_type->fn_call = [](Object *self, Object *args, Object *kwargs) -> Object * {
        auto me = reinterpret_cast<Type *>(self);

        if (!me->constructor) {
            throw_fmt(NameError, "Type %s has no constructor", me->name.c_str());

            return nullptr;
        }

        return me->constructor(self, args, kwargs);
    };

    // @str
    class_type->fn_str = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Type *>(self);
        auto result = new (nothrow) Str("Type(" + me->name + ")");

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };
}

bool Type::operator==(const Type &other) const { return id == other.id; }
