#include "frame.hh"
#include "error.hh"

using namespace std;

Type *Frame::class_type = nullptr;

Frame::Frame() : Object(Frame::class_type) {}

void Frame::init_class_type() {
    class_type = new (nothrow) Type("Frame");
    if (!class_type) {
        THROW_MEMORY_ERROR;
        return;
    }

    class_type->fn_traverse_objects = [](Object *self,
                                         const fn_visit_object_t &visit) {
        Frame *me = reinterpret_cast<Frame *>(self);

        visit(me->scope);
        visit(me->previous);
    };
}
