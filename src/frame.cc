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

        // Can be nullptr
        visit(me->previous);
    };

    class_type->fn_getitem = [](Object *self, Object *key) -> Object* {
        Frame *me = reinterpret_cast<Frame *>(self);

        return me->fetch(key);
    };

    class_type->fn_setitem = [](Object *self, Object *key, Object *value) -> Object* {
        Frame *me = reinterpret_cast<Frame *>(self);

        return me->vars->setitem(key, value);
    };

    class_type->fn_str = [](Object *self) -> Object* {
        Frame *me = reinterpret_cast<Frame *>(self);

        return me->vars->str();
    };
}

size_t Frame::lineof(size_t offset) {
    size_t l = start_lineno;

    for (auto it = line_deltas.begin(); it != line_deltas.end(); ++it) {
        const auto &[off, delta] = *it;

        if (off <= offset) l += delta;
        else break;
    }

    return l;
}
