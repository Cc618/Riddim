#include "object.hh"
// TODO
#include <iostream>

using namespace std;

// --- Object ---
Object::Object() {
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

Type::Type(const str_t &name) : name(name) { id = ++type_global_id; }

// --- Globals ---
Type *Type::class_type = nullptr;
Type *Object::class_type = nullptr;

// --- TODO ---
struct TestObject : public Object {
    TestObject(const str_t &id = "?", Type *type = nullptr) : id(id) {
        this->type = type;
    }

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
    auto maintype = new TestType();
    auto parent = new TestObject("root", maintype);

    auto a = new TestObject("a", maintype);
    auto b = new TestObject("b", maintype);
    auto c = new TestObject("c", maintype);
    auto d = new TestObject("d", maintype);

    auto e = new TestObject("e", maintype);
    auto f = new TestObject("f", maintype);

    parent->children.insert(parent->children.begin(), {a, b, c});
    c->children.push_back(d);
    d->children.push_back(c);

    e->children.push_back(f);

    cout << "Collecting garbages" << endl;
    garbage_collect(parent);

    // --- Object Test ---
    // auto type = new MyType();
    // type->traverse_objects([](Object *o) { cout << o << endl; });
}
