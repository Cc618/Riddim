#include "object.hh"
// TODO
#include <iostream>

using namespace std;

Object *Object::Global() {
    return new Object();
}

Object *Object::Local() {
    auto o = new Object();
    init_gc_data(o);

    return o;
}

Object::Object() {}

Object::~Object() {
    // TODO : Delete function (of the type)
    cout << "Object deleted" << endl;
}

void testObjects() {
    Object *parent = Object::Global();

    Object *a = Object::Local();
    Object *b = Object::Local();
    Object *c = Object::Local();
    Object *d = Object::Local();

    Object *e = Object::Local();
    Object *f = Object::Global();

    parent->children.insert(parent->children.begin(), {a,b,c});
    c->children.push_back(d);
    d->children.push_back(c);
    e->children.push_back(f);

    cout << "Collecting garbages" << endl;
    garbage_collect(parent);
}
