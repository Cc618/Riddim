#include "object.hh"
// TODO
#include <iostream>

using namespace std;

// --- Object ---
// TODO : Out of memory exception
Object *Object::Global() { return new Object(); }

// TODO : Out of memory exception
Object *Object::Local() {
    auto o = new Object();

    init_gc_data(o);

    return o;
}

Object::Object() {}

Object::~Object() {
    // TODO : Call delete function (of the type)
}

// --- Type ---
static int type_global_id = 0;

Type::Type(const str_t &name) : name(name) { id = ++type_global_id; }

// --- TODO ---
// struct MyType : public Type {
//     MyType() : Type("MyType") {
//         traverse_objects = [this](Object *self, function<void(Object * o)> visit) -> void {
//             visit(a);
//             visit(b);
//             visit(c);
//         };

//         a = Object::Local();
//         b = Object::Local();
//         c = Object::Local();
//     }

//     Object *a, *b, *c;
// };

struct TestObject : public Object {
    TestObject(const str_t &id = "?", Type *type = nullptr) : id(id) {
        init_gc_data(this);

        this->type = type;
    }

    ~TestObject() { cout << id << " deleted" << endl; }

    str_t id;

    vector<Object *> children;
};

struct TestType : public Type {
    TestType() : Type("TestType") {
        traverse_objects = [this](Object *self,
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
