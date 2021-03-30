#include "object.hh"
#include "program.hh"
// TODO
#include <iostream>

using namespace std;

// --- Object ---
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

// --- Type ---
static int type_global_id = 0;

Type::Type(const str_t &name) : Object(Type::class_type), name(name) {
    // Register this type to the program
    if (Program::instance) Program::instance->types.push_back(this);

    id = ++type_global_id;
}

// --- Globals ---
Type *Type::class_type = nullptr;
Type *Object::class_type = nullptr;

// --- TODO ---
struct TestType;
static TestType *test_type = nullptr;

struct TestObject : public Object {
    TestObject(const str_t &id = "?") : Object((Type*)test_type), id(id) {}

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

void testObjects() {
    // --- GC test ---
    test_type = new TestType();
    auto parent = new TestObject("root");

    // Keep parent and its children alive
    Program::instance->globals.push_back(parent);

    auto a = new TestObject("a");
    auto b = new TestObject("b");
    auto c = new TestObject("c");
    auto d = new TestObject("d");

    auto e = new TestObject("e");
    auto f = new TestObject("f");

    parent->children.insert(parent->children.begin(), {a, b, c});
    c->children.push_back(d);
    d->children.push_back(c);

    e->children.push_back(f);

    // cout << "Collecting garbages" << endl;
    // garbage_collect(parent);

    // --- Object Test ---
    // auto type = new MyType();
    // type->traverse_objects([](Object *o) { cout << o << endl; });
}
