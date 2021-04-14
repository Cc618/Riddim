#include "trace.hh"
#include "error.hh"
#include "program.hh"
#include "str.hh"
#include "utils.hh"
#include <iostream>

using namespace std;

Type *Trace::class_type = nullptr;

Trace *Trace::New(size_t ip, Code *code, const str_t &id) {
    auto self = new (nothrow) Trace(ip, code, id);

    if (!self) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    return self;
}

Trace::Trace(size_t ip, Code *code, const str_t &id)
    : Object(Trace::class_type), ip(ip), code(code), id(id) {}

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

void Trace::display() {
    auto line = code->lineof(ip);

    cerr << C_GREEN << code->filename << ":" << C_RED << to_string(line) << " " << C_BLUE << id << C_NORMAL << endl;
    show_source_line(code->filename, line);
}

void Trace::dump() {
    auto current = this;

    int level = 1;
    while (current) {
        cerr << "  (#" << level << ") ";
        current->display();

        current = current->prev;
        ++level;
    }
}
