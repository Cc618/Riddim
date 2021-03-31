#include "object.hh"
#include "error.hh"
#include "program.hh"
#include "str.hh"
#include <iostream>

using namespace std;

// --- Object ---
Type *Object::class_type = nullptr;

void Object::init_class_type() {
    class_type = new Type("Object");
}

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

// Throws a NameError that says no such builtin method
#define THROW_NOBUILTIN(METHOD)                                                \
    throw_fmt(NameError, "Type '%s' has no @" #METHOD " method",               \
              type->name.c_str());

Object *Object::index(Object *args) {
    if (!type->fn_index) {
        THROW_NOBUILTIN(index);

        return nullptr;
    }

    return type->fn_index(this, args);
}

Object *Object::hash() {
    if (!type->fn_hash) {
        THROW_NOBUILTIN(hash);

        return nullptr;
    }

    return type->fn_hash(this);
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
    class_type->fn_str = [](Object *self) -> Object* {
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
#include "map.hh"
#include "int.hh"
#include "str.hh"

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
    cout << reinterpret_cast<Str*>(result)->data << endl;
}

void testObjects() {
    Int *integer = new Int(42);
    Str *str = new Str("Hello Riddim !!!");

    // integer->index(str); : NameError
    Int::class_type->fn_str = decltype(Int::class_type->fn_str)();
    print(integer->type);
    // print(str);

    /*
    HashMap *map = new HashMap();
    Object *key = new Type("Key");
    Object *value = new Type("Value");

    map->data[key] = value;

    cout << "Index 1 : " << map->fn_index(key) << endl;
    cout << "Index 2 : " << map->fn_index(value) << endl;
    cerr << "Error : " << ((Error *)Program::instance->current_error)->msg
         << endl;
    */
}
