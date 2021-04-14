#include "trace.hh"
#include "error.hh"
#include "program.hh"
#include "str.hh"

using namespace std;

Type *Trace::class_type = nullptr;

Trace *Trace::New(size_t ip, Code *code) {
    auto self = new (nothrow) Trace(ip, code);

    if (!self) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    return self;
}

Trace::Trace(size_t ip, Code *code)
    : Object(Trace::class_type), ip(ip), code(code) {}

void Trace::init_class_type() {
    class_type = new (nothrow) Type("Trace");

    if (!class_type) {
        THROW_MEMORY_ERROR;

        return;
    }

    class_type->fn_traverse_objects = [](Object *self, const fn_visit_object_t &visit) {
        Trace *me = reinterpret_cast<Trace *>(self);

        visit(me->code);

        // Can be nullptr
        visit(me->prev);
    };
}
