#include "frame.hh"
#include "bool.hh"
#include "str.hh"
#include "error.hh"

using namespace std;

Type *Frame::class_type = nullptr;

Frame::Frame(Frame *previous) : Object(Frame::class_type), previous(previous) {}

Object *Frame::fetch(Object *name) {
    if (name->type != Str::class_type) {
        THROW_TYPE_ERROR_PREF("Frame.fetch", name->type, Str::class_type);

        return nullptr;
    }

    Frame *frame = this;

    while (frame) {
        auto found = frame->vars->find(name);

        // Found
        if (found != frame->vars->data.end()) {
            const auto &[h, kv] = *found;
            const auto &[k, v] = kv;

            return v;
        }

        frame = frame->previous;
    }

    THROW_NAME_ERROR(reinterpret_cast<Str *>(name)->data);

    return nullptr;
}

Frame *Frame::New(Frame *previous) {
    auto o = new (nothrow) Frame(previous);

    if (!o) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    o->vars = new (nothrow) HashMap();

    if (!o->vars) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    return o;
}

void Frame::init_class_type() {
    class_type = new (nothrow) Type("Frame");
    if (!class_type) {
        THROW_MEMORY_ERROR;
        return;
    }

    class_type->fn_traverse_objects = [](Object *self,
                                         const fn_visit_object_t &visit) {
        Frame *me = reinterpret_cast<Frame *>(self);

        visit(me->vars);
        visit(me->previous);
    };
}
