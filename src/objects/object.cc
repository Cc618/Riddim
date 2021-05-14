#include "object.hh"
#include "bool.hh"
#include "doc.hh"
#include "error.hh"
#include "function.hh"
#include "hash.hh"
#include "int.hh"
#include "null.hh"
#include "program.hh"
#include "str.hh"
#include <iostream>

using namespace std;

bool is_type(Object *o) {
    return o->type == Type::class_type || o->type == DynamicType::class_type;
}

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

Object::Object(Type *type) : type(type) { init_gc_data(this); }

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

Object *Object::doc() {
    if (!type->fn_doc) {
        return null;
    }

    auto result = type->fn_doc(this);

    if (!result) {
        return nullptr;
    }

    // Must be Str
    if (result->type != Null::class_type && result->type != Str::class_type) {
        THROW_TYPE_ERROR_PREF((type->name + "@doc").c_str(), result->type,
                              Str::class_type);

        return nullptr;
    }

    return result;
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

    if (!result) {
        return nullptr;
    }

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

static auto type_default_call(Object *self, Object *args, Object *kwargs)
    -> Object * {
    auto me = reinterpret_cast<Type *>(self);

    if (!me->constructor) {
        throw_fmt(NameError, "Type %s has no constructor", me->name.c_str());

        return nullptr;
    }

    return me->constructor(self, args, kwargs);
}

void Type::init_slots(Type *type) {
    // @call
    // Call the constructor
    type->fn_call = type_default_call;

    // @getattr
    type->fn_getattr = [](Object *self, Object *name) -> Object * {
        auto me = reinterpret_cast<Type *>(self);

        if (name->type != Str::class_type) {
            THROW_TYPE_ERROR_PREF("Type.@getattr", name->type, Str::class_type);

            return nullptr;
        }

        auto attr = reinterpret_cast<Str *>(name)->data;

        // TODO : !name
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
        // No special attribute variables
        if (k.empty() || k[0] == '!') {
            continue;
        }

        auto newv = v->copy();

        if (!newv) {
            return;
        }

        // If function, bind self
        if (newv->type == Builtin::class_type ||
            newv->type == Function::class_type) {
            auto newv_fun = dynamic_cast<AbstractFunction *>(newv);

            if (!newv_fun) {
                throw_fmt(
                    RuntimeError,
                    "DynamicObject.@new got an invalid function instance");

                return;
            }

            newv_fun->self = self;
        }

        self->attrs[k] = newv;
    }
}

Object *DynamicObject::dynamic_model_copy() {
    // To respect the protocol and the style of the project
    auto self = this;
    auto me = this;

    unordered_map<str_t, Object *> attrs;
    for (const auto &[k, v] : me->attrs) {
        auto newval = v->copy();

        if (!newval) {
            return nullptr;
        }

        attrs[k] = newval;
    }

    auto copy = new (nothrow) DynamicObject(self->type);

    if (!copy) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    copy->attrs = move(attrs);

    return copy;
}

// --- DynamicType ---
void DynamicType::default_traverse_objects(Object *self,
                                           const fn_visit_object_t &visit) {
    auto me = dynamic_cast<DynamicModel *>(self);

    if (!me) {
        throw_fmt(RuntimeError,
                  "DynamicModel.@traverse got an invalid self instance");

        return;
    }

    for (const auto &[k, v] : me->attrs)
        visit(v);
}

DynamicType *DynamicType::New(const str_t &name) {
    auto me = new (nothrow) DynamicType(name);

    if (!me) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    if (DynamicType::class_type && Str::class_type) {
        // Set !name
        auto sname = Str::New(name);

        if (!sname) {
            return nullptr;
        }

        me->attrs["!name"] = sname;
    }

    me->fn_traverse_objects = default_traverse_objects;

    // @add
    me->fn_add = [](Object *self, Object *other) -> Object * {
        auto me = dynamic_cast<DynamicModel *>(self);

        if (!me) {
            throw_fmt(RuntimeError,
                      "DynamicModel.@add got an invalid self instance");

            return nullptr;
        }

        auto it = me->attrs.find("@add");
        if (it != me->attrs.end()) {
            auto args = Vec::New({other});

            if (!args) {
                return nullptr;
            }

            return it->second->call(args, HashMap::empty);
        }

        THROW_NOBUILTIN(self->type, add);

        return nullptr;
    };

    // @call
    me->fn_call = [](Object *self, Object *args, Object *kwargs) -> Object * {
        auto me = dynamic_cast<DynamicModel *>(self);

        if (!me) {
            throw_fmt(RuntimeError,
                      "DynamicModel.@call got an invalid self instance");

            return nullptr;
        }

        auto it = me->attrs.find("@call");
        if (it != me->attrs.end()) {
            return it->second->call(args, kwargs);
        }

        THROW_NOBUILTIN(self->type, call);

        return nullptr;
    };

    // @cmp
    me->fn_cmp = [](Object *self, Object *other) -> Object * {
        auto me = dynamic_cast<DynamicModel *>(self);

        if (!me) {
            throw_fmt(RuntimeError,
                      "DynamicModel.@cmp got an invalid self instance");

            return nullptr;
        }

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

    // @copy
    me->fn_copy = [](Object *self) -> Object * {
        auto me = dynamic_cast<DynamicModel *>(self);

        if (!me) {
            throw_fmt(RuntimeError,
                      "DynamicModel.@copy got an invalid self instance");

            return nullptr;
        }

        return me->dynamic_model_copy();
    };

    // @div
    me->fn_div = [](Object *self, Object *other) -> Object * {
        auto me = dynamic_cast<DynamicModel *>(self);

        if (!me) {
            throw_fmt(RuntimeError,
                      "DynamicModel.@div got an invalid self instance");

            return nullptr;
        }

        auto it = me->attrs.find("@div");
        if (it != me->attrs.end()) {
            auto args = Vec::New({other});

            if (!args) {
                return nullptr;
            }

            return it->second->call(args, HashMap::empty);
        }

        THROW_NOBUILTIN(self->type, div);

        return nullptr;
    };

    // @doc
    me->fn_doc = [](Object *self) -> Object * {
        auto me = dynamic_cast<DynamicModel *>(self);

        if (!me) {
            throw_fmt(RuntimeError,
                      "DynamicModel.@doc got an invalid self instance");

            return nullptr;
        }

        // Str* or Null* guaranteed
        Object *current_doc = nullptr;

        auto it = me->attrs.find("@doc");
        if (it != me->attrs.end()) {
            current_doc = it->second->call(Vec::empty, HashMap::empty);

            if (!current_doc) {
                return nullptr;
            }
        }

        if (!current_doc) {
            // The !doc attribute may contain the documentation
            auto it2 = me->attrs.find("!doc");
            if (it2 != me->attrs.end()) {
                current_doc = it2->second;

                if (!current_doc) {
                    return nullptr;
                }
            }
        }

        if (current_doc && current_doc->type != Str::class_type) {
            THROW_TYPE_ERROR_PREF((current_doc->type->name + "@str").c_str(),
                                  current_doc->type, Str::class_type);

            return nullptr;
        }

        str_t result;
        str_t name_data;

        {
            auto it = me->attrs.find("!name");
            if (it != me->attrs.end()) {
                auto name = it->second;

                if (name->type != Str::class_type) {
                    THROW_TYPE_ERROR_PREF("!name", name->type, Str::class_type);

                    return nullptr;
                }

                name_data = reinterpret_cast<Str *>(name)->data;
            }
        }

        // Filter what to document
        vector<pair<str_t, Object *>> children;
        for (const auto &[child_name, child] : me->attrs) {
            if (!is_special_var(child_name, false)) {
                children.push_back({child_name, child});
            }
        }

        result = autodoc(2, name_data,
                         !current_doc || current_doc == null
                             ? ""
                             : reinterpret_cast<Str *>(current_doc)->data,
                         children);

        if (on_error()) {
            return nullptr;
        }

        if (result.empty()) {
            return null;
        }

        auto sresult = Str::New(result);

        if (!sresult) {
            return nullptr;
        }

        return sresult;
    };

    // @getattr
    me->fn_getattr = [](Object *self, Object *name) -> Object * {
        auto me = dynamic_cast<DynamicModel *>(self);

        if (!me) {
            throw_fmt(RuntimeError,
                      "DynamicModel.@getattr got an invalid self instance");

            return nullptr;
        }

        if (name->type != Str::class_type) {
            THROW_TYPE_ERROR_PREF((self->type->name + ".@getattr").c_str(),
                                  name->type, Str::class_type);

            return nullptr;
        }

        auto attr = reinterpret_cast<Str *>(name)->data;

        // Find target attribute
        auto result_it = me->attrs.find(attr);
        if (result_it == me->attrs.end()) {
            // No such attribute
            THROW_ATTR_ERROR(self->type, attr);

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
        auto me = dynamic_cast<DynamicModel *>(self);

        if (!me) {
            throw_fmt(RuntimeError,
                      "DynamicModel.@getitem got an invalid self instance");

            return nullptr;
        }

        auto it = me->attrs.find("@getitem");
        if (it != me->attrs.end()) {
            auto args = Vec::New({key});

            if (!args) {
                return nullptr;
            }

            return it->second->call(args, HashMap::empty);
        }

        THROW_NOBUILTIN(self->type, getitem);

        return nullptr;
    };

    // @hash
    me->fn_hash = [](Object *self) -> Object * {
        auto me = dynamic_cast<DynamicModel *>(self);

        if (!me) {
            throw_fmt(RuntimeError,
                      "DynamicModel.@hash got an invalid self instance");

            return nullptr;
        }

        auto it = me->attrs.find("@hash");
        if (it != me->attrs.end()) {
            return it->second->call(Vec::empty, HashMap::empty);
        }

        return default_hash(self);
    };

    // @in
    me->fn_in = [](Object *self, Object *other) -> Object * {
        auto me = dynamic_cast<DynamicModel *>(self);

        if (!me) {
            throw_fmt(RuntimeError,
                      "DynamicModel.@in got an invalid self instance");

            return nullptr;
        }

        auto it = me->attrs.find("@in");
        if (it != me->attrs.end()) {
            auto args = Vec::New({other});

            if (!args) {
                return nullptr;
            }

            return it->second->call(args, HashMap::empty);
        }

        THROW_NOBUILTIN(self->type, in);

        return nullptr;
    };

    // @iter
    me->fn_iter = [](Object *self) -> Object * {
        auto me = dynamic_cast<DynamicModel *>(self);

        if (!me) {
            throw_fmt(RuntimeError,
                      "DynamicModel.@iter got an invalid self instance");

            return nullptr;
        }

        auto it = me->attrs.find("@iter");
        if (it != me->attrs.end()) {
            return it->second->call(Vec::empty, HashMap::empty);
        }

        THROW_NOBUILTIN(self->type, iter);

        return nullptr;
    };

    // @len
    me->fn_len = [](Object *self) -> Object * {
        auto me = dynamic_cast<DynamicModel *>(self);

        if (!me) {
            throw_fmt(RuntimeError,
                      "DynamicModel.@len got an invalid self instance");

            return nullptr;
        }

        auto it = me->attrs.find("@len");
        if (it != me->attrs.end()) {
            return it->second->call(Vec::empty, HashMap::empty);
        }

        THROW_NOBUILTIN(self->type, len);

        return nullptr;
    };

    // @mod
    me->fn_mod = [](Object *self, Object *other) -> Object * {
        auto me = dynamic_cast<DynamicModel *>(self);

        if (!me) {
            throw_fmt(RuntimeError,
                      "DynamicModel.@mod got an invalid self instance");

            return nullptr;
        }

        auto it = me->attrs.find("@mod");
        if (it != me->attrs.end()) {
            auto args = Vec::New({other});

            if (!args) {
                return nullptr;
            }

            return it->second->call(args, HashMap::empty);
        }

        THROW_NOBUILTIN(self->type, mod);

        return nullptr;
    };

    // @mul
    me->fn_mul = [](Object *self, Object *other) -> Object * {
        auto me = dynamic_cast<DynamicModel *>(self);

        if (!me) {
            throw_fmt(RuntimeError,
                      "DynamicModel.@mul got an invalid self instance");

            return nullptr;
        }

        auto it = me->attrs.find("@mul");
        if (it != me->attrs.end()) {
            auto args = Vec::New({other});

            if (!args) {
                return nullptr;
            }

            return it->second->call(args, HashMap::empty);
        }

        THROW_NOBUILTIN(self->type, mul);

        return nullptr;
    };

    // @neg
    me->fn_neg = [](Object *self) -> Object * {
        auto me = dynamic_cast<DynamicModel *>(self);

        if (!me) {
            throw_fmt(RuntimeError,
                      "DynamicModel.@neg got an invalid self instance");

            return nullptr;
        }

        auto it = me->attrs.find("@neg");
        if (it != me->attrs.end()) {
            return it->second->call(Vec::empty, HashMap::empty);
        }

        THROW_NOBUILTIN(self->type, neg);

        return nullptr;
    };

    // @next
    me->fn_next = [](Object *self) -> Object * {
        auto me = dynamic_cast<DynamicModel *>(self);

        if (!me) {
            throw_fmt(RuntimeError,
                      "DynamicModel.@next got an invalid self instance");

            return nullptr;
        }

        auto it = me->attrs.find("@next");
        if (it != me->attrs.end()) {
            return it->second->call(Vec::empty, HashMap::empty);
        }

        THROW_NOBUILTIN(self->type, next);

        return nullptr;
    };

    // @setattr
    me->fn_setattr = [](Object *self, Object *key, Object *val) -> Object * {
        auto me = dynamic_cast<DynamicModel *>(self);

        if (!me) {
            throw_fmt(RuntimeError,
                      "DynamicModel.@setattr got an invalid self instance");

            return nullptr;
        }

        if (key->type != Str::class_type) {
            THROW_TYPE_ERROR_PREF((self->type->name + ".@setattr").c_str(),
                                  key->type, Str::class_type);

            return nullptr;
        }

        auto attr = reinterpret_cast<Str *>(key)->data;

        me->attrs[attr] = val;

        return null;
    };

    // @setitem
    me->fn_setitem = [](Object *self, Object *key, Object *value) -> Object * {
        auto me = dynamic_cast<DynamicModel *>(self);

        if (!me) {
            throw_fmt(RuntimeError,
                      "DynamicModel.@setitem got an invalid self instance");

            return nullptr;
        }

        auto it = me->attrs.find("@setitem");
        if (it != me->attrs.end()) {
            auto args = Vec::New({key, value});

            if (!args) {
                return nullptr;
            }

            return it->second->call(args, HashMap::empty);
        }

        THROW_NOBUILTIN(self->type, setitem);

        return nullptr;
    };

    // @str
    me->fn_str = [](Object *self) -> Object * {
        auto me = dynamic_cast<DynamicModel *>(self);

        if (!me) {
            throw_fmt(RuntimeError,
                      "DynamicModel.@str got an invalid self instance");

            return nullptr;
        }

        auto it = me->attrs.find("@str");
        if (it != me->attrs.end()) {
            return it->second->call(Vec::empty, HashMap::empty);
        }

        // Default print
        str_t result = self->type->name;

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
        auto me = dynamic_cast<DynamicModel *>(self);

        if (!me) {
            throw_fmt(RuntimeError,
                      "DynamicModel.@sub got an invalid self instance");

            return nullptr;
        }

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
    // A dynamic type is a dynamic type
    class_type = DynamicType::New("DynamicType");

    if (!class_type) {
        return;
    }

    // Was not set
    class_type->type = class_type;

    // Inherit from super type (Type)
    class_type->fn_call = type_default_call;
}

DynamicType::DynamicType(const str_t &name) : Type(name) {
    // Override type
    type = DynamicType::class_type;
}

// No deep copy
Object *DynamicType::dynamic_model_copy() { return this; }
