#include "object.hh"
// TODO
#include <iostream>

using namespace std;

Object::Object() {
    init_gc_data(this);
}

Object::~Object() { cout << "Object deleted" << endl; }

void testObjects() {
    Object *parent = new Object();

    Object *a = new Object();
    Object *b = new Object();
    Object *c = new Object();
    Object *d = new Object();

    Object *e = new Object();
    Object *f = new Object();

    parent->children.insert(parent->children.begin(), {a,b,c});
    c->children.push_back(d);
    d->children.push_back(c);
    e->children.push_back(f);

    cout << "Collecting garbages" << endl;
    garbage_collect(parent);
}
