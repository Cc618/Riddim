#include "object.hh"
#include "bool.hh"
#include "error.hh"
#include "function.hh"
#include "hash.hh"
#include "int.hh"
#include "null.hh"
#include "program.hh"
#include "str.hh"
#include <iostream>

using namespace std;

bool is_pod_object(Object *o) {
    if (o->type == Int::class_type)
        return true;
    if (o->type == Bool::class_type)
        return true;

    return false;
}

// --- Object ---
Type *Object::class_type = nullptr;

void Object::init_class_type() { class_type = new Type("Object"); }

Object::Object(Type *type) : type(type) {
    init_gc_data(this);
}

Object::~Object() {}

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

static Object *default_cmp(Object *self, Object *o) {
    bool iseq = self == o;

    auto result = new (nothrow) Int(iseq ? 0 : -1);

    if (!result) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    return result;
}

Object *Object::cmp(Object *o) {
    if (!type->fn_cmp) {
        return default_cmp(this, o);
    }

    auto result = type->fn_cmp(this, o);

    if (!result)
        return nullptr;

    // Must be int
    if (result->type != Int::class_type) {
        THROW_TYPE_ERROR_PREF((type->name + "@cmp").c_str(), result->type,
                              Int::class_type);

        return nullptr;
    }

    return result;
}

static Object *default_copy(Object *self) { return self; }

Object *Object::copy() {
    if (!type->fn_copy) {
        return default_copy(this);
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

static Object *default_getattr(Object *self, Object *name) {
    auto name_str = name->str();

    if (!name_str || name_str->type != Str::class_type) {
        clear_error();
        throw_fmt(NameError, "No such attribute for type %s", self->type);
    } else
        THROW_ATTR_ERROR(self->type, reinterpret_cast<Str *>(name_str)->data);

    return nullptr;
}

Object *Object::getattr(Object *name) {
    if (!type->fn_getattr) {
        return default_getattr(this, name);
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

static Object *default_hash(Object *self) {
    auto result =
        new (nothrow) Int(hash_combine(hash_ptr(self), hash_ptr(self->type)));

    if (!result) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    return result;
}

Object *Object::hash() {
    // Default hash using the object memory address and its type
    if (!type->fn_hash) {
        return default_hash(this);
    }

    auto result = type->fn_hash(this);

    if (result->type != Int::class_type) {
        THROW_TYPE_ERROR_PREF((type->name + ".@hash").c_str(), result->type,
                              Int::class_type);

        return nullptr;
    }

    return result;
}

Object *Object::in(Object *val) {
    if (!type->fn_in) {
        THROW_NOBUILTIN(this->type, in);

        return nullptr;
    }

    return type->fn_in(this, val);
}

Object *Object::iter() {
    if (!type->fn_iter) {
        THROW_NOBUILTIN(this->type, iter);

        return nullptr;
    }

    return type->fn_iter(this);
}

Object *Object::len() {
    if (!type->fn_len) {
        THROW_NOBUILTIN(this->type, len);

        return nullptr;
    }

    auto result = type->fn_len(this);

    if (!result)
        return nullptr;

    // Must be int
    if (result->type != Int::class_type) {
        THROW_TYPE_ERROR_PREF((type->name + "@len").c_str(), result->type,
                              Int::class_type);

        return nullptr;
    }

    return result;
}

Object *Object::mod(Object *o) {
    if (!type->fn_mod) {
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

Object *Object::next() {
    if (!type->fn_next) {
        THROW_NOBUILTIN(this->type, next);

        return nullptr;
    }

    return type->fn_next(this);
}

static Object *default_setattr(Object *self, Object *name, Object *value) {
    auto name_str = name->str();

    if (!name_str || name_str->type != Str::class_type) {
        clear_error();
        throw_fmt(NameError, "No such attribute for type %s", self->type);
    } else
        THROW_ATTR_ERROR(self->type, reinterpret_cast<Str *>(name_str)->data);

    return nullptr;
}

Object *Object::setattr(Object *name, Object *value) {
    if (!type->fn_setattr) {
        return default_setattr(this, name, value);
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

static Object *default_str(Object *self) {
    auto result = Str::New(self->type->name + "()");

    if (!result) {
        return nullptr;
    }

    return result;
}

Object *Object::str() {
    if (!type->fn_str) {
        return default_str(this);
    }

    auto result = type->fn_str(this);

    if (!result)
        return nullptr;

    // Must be str
    if (result->type != Str::class_type) {
        THROW_TYPE_ERROR_PREF((type->name + "@str").c_str(), result->type,
                              Str::class_type);

        return nullptr;
    }

    return result;
}

static Object *default_sub(Object *self, Object *o) {
    // a - b == a + (-b)
    if (self->type->fn_add && self->type->fn_neg) {
        auto negated = o->neg();

        if (!negated)
            return nullptr;

        auto result = self->add(negated);

        if (!result)
            return nullptr;

        return result;
    }

    THROW_NOBUILTIN(self->type, sub);

    return nullptr;
}

Object *Object::sub(Object *o) {
    if (!type->fn_sub) {
        return default_sub(this, o);
    }

    return type->fn_sub(this, o);
}

// --- Type ---
static int type_global_id = 0;

Type *Type::class_type = nullptr;

Type::Type(const str_t &name, bool register_type)
    : Object(Type::class_type), name(name) {
    // Register this type to the program
    if (register_type && Program::instance)
        Program::add_type(this);

    id = ++type_global_id;
}

void Type::init_class_type() {
    class_type = new Type("Type");

    // Was not initialized
    class_type->type = class_type;

    init_slots(class_type);
}

bool Type::operator==(const Type &other) const { return id == other.id; }

void Type::init_slots(Type *type) {
    // @call
    // Call the constructor
    type->fn_call = [](Object *self, Object *args, Object *kwargs) -> Object * {
        auto me = reinterpret_cast<Type *>(self);

        if (!me->constructor) {
            throw_fmt(NameError, "Type %s has no constructor",
                      me->name.c_str());

            return nullptr;
        }

        return me->constructor(self, args, kwargs);
    };

    // @getattr
    type->fn_getattr = [](Object *self, Object *name) -> Object * {
        auto me = reinterpret_cast<Type *>(self);

        if (name->type != Str::class_type) {
            THROW_TYPE_ERROR_PREF("Type.@getattr", name->type, Str::class_type);

            return nullptr;
        }

        auto attr = reinterpret_cast<Str *>(name)->data;

        if (attr == "name") {
            auto result = Str::New(me->name);

            if (!result) {
                return nullptr;
            }

            return result;
        }

        THROW_ATTR_ERROR(me->type, attr);

        return nullptr;
    };

    // @str
    type->fn_str = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Type *>(self);
        auto result = Str::New("Type(" + me->name + ")");

        if (!result) {
            return nullptr;
        }

        return result;
    };
}

// --- DynamicObject ---
void DynamicObject::init(DynamicObject *self) {
    // Add attributes and methods
    for (const auto &[k, v] :
         reinterpret_cast<DynamicType *>(self->type)->attrs) {
        auto newv = v->copy();

        if (!newv) {
            return;
        }

        // If function, bind self
        if (newv->type == Builtin::class_type ||
            newv->type == Function::class_type) {
            auto newv_fun = reinterpret_cast<AbstractFunction *>(newv);
            newv_fun->self = self;
        }

        self->attrs[k] = newv;
    }
}

// --- DynamicType ---
void DynamicType::default_traverse_objects(Object *self,
                                           const fn_visit_object_t &visit) {
    auto me = reinterpret_cast<DynamicObject *>(self);

    for (const auto &[k, v] : me->attrs)
        visit(v);
}

DynamicType *DynamicType::New(const str_t &name) {
    auto me = new (nothrow) DynamicType(name);

    if (!me) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    me->fn_traverse_objects = default_traverse_objects;

    // @add
    me->fn_add = [](Object *self, Object *other) -> Object * {
        auto me = reinterpret_cast<DynamicObject *>(self);

        auto it = me->attrs.find("@add");
        if (it != me->attrs.end()) {
            auto args = Vec::New({other});

            if (!args) {
                return nullptr;
            }

            return it->second->call(args, HashMap::empty);
        }

        THROW_NOBUILTIN(me->type, add);

        return nullptr;
    };

    // @call
    me->fn_call = [](Object *self, Object *args, Object *kwargs) -> Object * {
        auto me = reinterpret_cast<DynamicObject *>(self);

        auto it = me->attrs.find("@call");
        if (it != me->attrs.end()) {
            return it->second->call(args, kwargs);
        }

        THROW_NOBUILTIN(me->type, call);

        return nullptr;
    };

    // @cmp
    me->fn_cmp = [](Object *self, Object *other) -> Object * {
        auto me = reinterpret_cast<DynamicObject *>(self);

        auto it = me->attrs.find("@cmp");
        if (it != me->attrs.end()) {
            auto args = Vec::New({other});

            if (!args) {
                return nullptr;
            }

            return it->second->call(args, HashMap::empty);
        }

        return default_cmp(self, other);
    };

    // @div
    me->fn_div = [](Object *self, Object *other) -> Object * {
        auto me = reinterpret_cast<DynamicObject *>(self);

        auto it = me->attrs.find("@div");
        if (it != me->attrs.end()) {
            auto args = Vec::New({other});

            if (!args) {
                return nullptr;
            }

            return it->second->call(args, HashMap::empty);
        }

        THROW_NOBUILTIN(me->type, div);

        return nullptr;
    };

    // @getattr
    me->fn_getattr = [](Object *self, Object *name) -> Object * {
        auto me = reinterpret_cast<DynamicObject *>(self);

        if (name->type != Str::class_type) {
            THROW_TYPE_ERROR_PREF((me->type->name + ".@getattr").c_str(),
                                  name->type, Str::class_type);

            return nullptr;
        }

        auto attr = reinterpret_cast<Str *>(name)->data;

        // Find target attribute
        auto result_it = me->attrs.find(attr);
        if (result_it == me->attrs.end()) {
            // No such attribute
            THROW_ATTR_ERROR(me->type, attr);

            return nullptr;
        }

        Object *result = result_it->second;

        // Check whether the object has been allocated
        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @getitem
    me->fn_getitem = [](Object *self, Object *key) -> Object * {
        auto me = reinterpret_cast<DynamicObject *>(self);

        auto it = me->attrs.find("@getitem");
        if (it != me->attrs.end()) {
            auto args = Vec::New({key});

            if (!args) {
                return nullptr;
            }

            return it->second->call(args, HashMap::empty);
        }

        THROW_NOBUILTIN(me->type, getitem);

        return nullptr;
    };

    // @hash
    me->fn_hash = [](Object *self) -> Object * {
        auto me = reinterpret_cast<DynamicObject *>(self);

        auto it = me->attrs.find("@hash");
        if (it != me->attrs.end()) {
            return it->second->call(Vec::empty, HashMap::empty);
        }

        return default_hash(self);
    };

    // @in
    me->fn_in = [](Object *self, Object *other) -> Object * {
        auto me = reinterpret_cast<DynamicObject *>(self);

        auto it = me->attrs.find("@in");
        if (it != me->attrs.end()) {
            auto args = Vec::New({other});

            if (!args) {
                return nullptr;
            }

            return it->second->call(args, HashMap::empty);
        }

        THROW_NOBUILTIN(me->type, in);

        return nullptr;
    };

    // @iter
    me->fn_iter = [](Object *self) -> Object * {
        auto me = reinterpret_cast<DynamicObject *>(self);

        auto it = me->attrs.find("@iter");
        if (it != me->attrs.end()) {
            return it->second->call(Vec::empty, HashMap::empty);
        }

        THROW_NOBUILTIN(me->type, iter);

        return nullptr;
    };

    // @len
    me->fn_len = [](Object *self) -> Object * {
        auto me = reinterpret_cast<DynamicObject *>(self);

        auto it = me->attrs.find("@len");
        if (it != me->attrs.end()) {
            return it->second->call(Vec::empty, HashMap::empty);
        }

        THROW_NOBUILTIN(me->type, len);

        return nullptr;
    };

    // @mod
    me->fn_mod = [](Object *self, Object *other) -> Object * {
        auto me = reinterpret_cast<DynamicObject *>(self);

        auto it = me->attrs.find("@mod");
        if (it != me->attrs.end()) {
            auto args = Vec::New({other});

            if (!args) {
                return nullptr;
            }

            return it->second->call(args, HashMap::empty);
        }

        THROW_NOBUILTIN(me->type, mod);

        return nullptr;
    };

    // @mul
    me->fn_mul = [](Object *self, Object *other) -> Object * {
        auto me = reinterpret_cast<DynamicObject *>(self);

        auto it = me->attrs.find("@mul");
        if (it != me->attrs.end()) {
            auto args = Vec::New({other});

            if (!args) {
                return nullptr;
            }

            return it->second->call(args, HashMap::empty);
        }

        THROW_NOBUILTIN(me->type, mul);

        return nullptr;
    };

    // @neg
    me->fn_neg = [](Object *self) -> Object * {
        auto me = reinterpret_cast<DynamicObject *>(self);

        auto it = me->attrs.find("@neg");
        if (it != me->attrs.end()) {
            return it->second->call(Vec::empty, HashMap::empty);
        }

        THROW_NOBUILTIN(me->type, neg);

        return nullptr;
    };

    // @next
    me->fn_next = [](Object *self) -> Object * {
        auto me = reinterpret_cast<DynamicObject *>(self);

        auto it = me->attrs.find("@next");
        if (it != me->attrs.end()) {
            return it->second->call(Vec::empty, HashMap::empty);
        }

        THROW_NOBUILTIN(me->type, next);

        return nullptr;
    };

    // @setattr
    me->fn_setattr = [](Object *self, Object *key, Object *val) -> Object * {
        auto me = reinterpret_cast<DynamicObject *>(self);

        if (key->type != Str::class_type) {
            THROW_TYPE_ERROR_PREF((me->type->name + ".@setattr").c_str(),
                                  key->type, Str::class_type);

            return nullptr;
        }

        auto attr = reinterpret_cast<Str *>(key)->data;

        me->attrs[attr] = val;

        return null;
    };

    // @setitem
    me->fn_setitem = [](Object *self, Object *key, Object *value) -> Object * {
        auto me = reinterpret_cast<DynamicObject *>(self);

        auto it = me->attrs.find("@setitem");
        if (it != me->attrs.end()) {
            auto args = Vec::New({key, value});

            if (!args) {
                return nullptr;
            }

            return it->second->call(args, HashMap::empty);
        }

        THROW_NOBUILTIN(me->type, setitem);

        return nullptr;
    };

    // @str
    me->fn_str = [](Object *self) -> Object * {
        auto me = reinterpret_cast<DynamicObject *>(self);

        auto it = me->attrs.find("@str");
        if (it != me->attrs.end()) {
            return it->second->call(Vec::empty, HashMap::empty);
        }

        // Default print
        str_t result = me->type->name;

        if (me->attrs.empty()) {
            result += "{}";
        } else {
            result += "{";
            bool isfirst = true;
            for (const auto &[k, v] : me->attrs) {
                // Don't display slots
                if (k.empty() || k[0] == '@')
                    continue;

                if (isfirst)
                    isfirst = false;
                else
                    result += ", ";

                // Print attributes
                auto v_str = v->str();

                // Error
                if (!v_str)
                    return nullptr;

                result += k + ": " + reinterpret_cast<Str *>(v_str)->data;
            }

            result += "}";
        }

        auto result_str = Str::New(result);

        if (!result_str) {
            return nullptr;
        }

        return result_str;
    };

    // @sub
    me->fn_sub = [](Object *self, Object *other) -> Object * {
        auto me = reinterpret_cast<DynamicObject *>(self);

        auto it = me->attrs.find("@sub");
        if (it != me->attrs.end()) {
            auto args = Vec::New({other});

            if (!args) {
                return nullptr;
            }

            return it->second->call(args, HashMap::empty);
        }

        return default_sub(self, other);
    };

    return me;
}

Type *DynamicType::class_type = nullptr;

void DynamicType::init_class_type() {
    class_type = new Type("DynamicType");

    // Inherit from super type (Type)
    Type::init_slots(class_type);

    class_type->fn_traverse_objects = [](Object *self,
                                         const fn_visit_object_t &visit) {
        auto me = reinterpret_cast<DynamicType *>(self);

        for (const auto &[k, v] : me->attrs)
            visit(v);
    };

    // @getattr
    class_type->fn_getattr = [](Object *self, Object *key) -> Object * {
        auto me = reinterpret_cast<DynamicType *>(self);

        if (key->type != Str::class_type) {
            THROW_TYPE_ERROR_PREF((me->type->name + ".@getattr").c_str(),
                                  key->type, Str::class_type);

            return nullptr;
        }

        auto attr = reinterpret_cast<Str *>(key)->data;

        // Find target attribute
        auto result_it = me->attrs.find(attr);
        if (result_it == me->attrs.end()) {
            // No such attribute
            THROW_ATTR_ERROR(Str::class_type, attr);

            return nullptr;
        }

        Object *result = result_it->second;

        // Check whether the object has been allocated
        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @setattr
    class_type->fn_setattr = [](Object *self, Object *key,
                                Object *val) -> Object * {
        auto me = reinterpret_cast<DynamicType *>(self);

        if (key->type != Str::class_type) {
            THROW_TYPE_ERROR_PREF((me->type->name + ".@setattr").c_str(),
                                  key->type, Str::class_type);

            return nullptr;
        }

        auto attr = reinterpret_cast<Str *>(key)->data;

        me->attrs[attr] = val;

        return null;
    };
}

DynamicType::DynamicType(const str_t &name) : Type(name) {
    // Override type
    type = DynamicType::class_type;
}
