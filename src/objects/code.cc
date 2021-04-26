#include "code.hh"
#include "bool.hh"
#include "error.hh"
#include "str.hh"

using namespace std;

Type *Code::class_type = nullptr;

Code::Code(const str_t &filename)
    : Object(Code::class_type), filename(filename) {}

Code *Code::New(const str_t &filename) {
    auto o = new (nothrow) Code(filename);

    if (!o) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    o->consts = Vec::New();

    // Dispatch error
    if (!o->consts) {
        return nullptr;
    }

    return o;
}

void Code::init_class_type() {
    class_type = new (nothrow) Type("Code");
    if (!class_type) {
        THROW_MEMORY_ERROR;
        return;
    }

    class_type->fn_traverse_objects = [](Object *self,
                                         const fn_visit_object_t &visit) {
        Code *me = reinterpret_cast<Code *>(self);

        visit(me->consts);
    };
}

size_t Code::lineof(size_t offset) {
    size_t l = start_lineno;

    for (auto it = line_deltas.begin(); it != line_deltas.end(); ++it) {
        const auto &[off, delta] = *it;

        if (off <= offset)
            l = delta;
        else
            break;
    }

    return l;
}

void Code::mark_line(size_t lineno) {
    if (line_deltas.empty())
        start_lineno = lineno;
    // Optimize if same instruction
    else if (line_deltas.back().first == code.size())
        line_deltas.pop_back();

    line_deltas.push_back({code.size(), lineno});
}

size_t Code::add_const(Object *cst) {
    consts->data.push_back(cst);

    return consts->data.size() - 1;
}

Object *Code::spawn_const(size_t i) {
    if (i >= consts->data.size()) {
        THROW_OUT_OF_BOUNDS(consts->data.size(), i);

        return nullptr;
    }

    return consts->data[i]->copy();
}
