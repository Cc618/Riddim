#include "object.hh"
#include "error.hh"
#include "hash.hh"
#include "int.hh"
#include "program.hh"
#include "str.hh"
#include <iostream>

using namespace std;

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
        THROW_NOBUILTIN(getitem);

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
        THROW_NOBUILTIN(setitem);

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

#undef THROW_NOBUILTIN

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

// --- TODO ---
#include "debug.hh"
#include "int.hh"
#include "map.hh"
#include "str.hh"
#include "frame.hh"
#include "bool.hh"

struct TestType;
static TestType *test_type = nullptr;

struct TestObject : public Object {
    TestObject(const str_t &id = "?") : Object((Type *)test_type), id(id) {}

    ~TestObject() { cout << id << " deleted" << endl; }

    str_t id;

    vector<Object *> children;
};

struct TestType : public Type {
    TestType() : Type("TestType") {
        fn_traverse_objects = [](Object *self,
                                 function<void(Object * o)> visit) -> void {
            for (auto child : ((TestObject *)self)->children)
                visit(child);
        };
    }

    ~TestType() { cout << "TestType deleted" << endl; }
};

void print(Object *o) {
    if (!o) {
        cout << "nullptr" << endl;
        return;
    }

    auto result = o->str();

    // TODO : Errors
    if (!result) {
        cout << "Can't find @str method" << endl;
        return;
    }
    if (result->type != Str::class_type) {
        cout << "Invalid @str type" << endl;
        return;
    }

    // TODO : Type of result error
    cout << reinterpret_cast<Str *>(result)->data << endl;
}

void testObjects() {
    // AttrObject *o = AttrObject::New();
    // o->setattr(new Str("a"), new Int(618));
    // o->setattr(new Str("b"), new Int(314));

    // print(o);
    // print(o->getattr(new Str("a")));
    // print(o->getattr(new Str("ab")));

    // auto err = Program::instance->current_error;
    // Program::instance->current_error = nullptr;

    // cout << "Message :" << endl;
    // print(err->getattr(new Str("msg")));

    print(istrue);
    print(isfalse);
}
