#include "trace.hh"
#include "error.hh"
#include "program.hh"
#include "str.hh"
#include <iostream>

using namespace std;

Type *Trace::class_type = nullptr;

Trace *Trace::New(size_t ip, Code *code, const str_t &id,
                  const str_t &filename) {
    auto self = new (nothrow) Trace(ip, code, id, filename);

    if (!self) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    return self;
}

Trace::Trace(size_t ip, Code *code, const str_t &id, const str_t &filename)
    : Object(Trace::class_type), ip(ip), code(code), id(id),
      filename(filename) {}

void Trace::init_class_type() {
    class_type = new (nothrow) Type("Trace");

    if (!class_type) {
        THROW_MEMORY_ERROR;

        return;
    }

    class_type->fn_traverse_objects = [](Object *self,
                                         const fn_visit_object_t &visit) {
        Trace *me = reinterpret_cast<Trace *>(self);

        visit(me->code);

        // Can be nullptr
        visit(me->prev);
    };
}

// TODO Colors : Error colors
str_t Trace::display() {
    return filename + ":" + to_string(code->lineof(ip)) + " " + id;
}

void Trace::dump(int level) {
    if (prev) prev->dump(level + 1);

    cerr << level << ". " << display() << endl;
}
