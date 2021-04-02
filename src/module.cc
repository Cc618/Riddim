#include "module.hh"
#include "error.hh"

using namespace std;

Type *Module::class_type = nullptr;

Module::Module(Str *name, Frame *frame)
    : Object(Module::class_type), name(name), frame(frame) {}

void Module::init_class_type() {
    class_type = new (nothrow) Type("Module");

    if (!class_type) {
        THROW_MEMORY_ERROR;
        return;
    }

    class_type->fn_traverse_objects = [](Object *self,
                                         const fn_visit_object_t &visit) {
        Module *me = reinterpret_cast<Module *>(self);

        visit(me->name);
        visit(me->frame);
    };

    class_type->fn_getitem = [](Object *self, Object *key) -> Object* {
        Module *me = reinterpret_cast<Module *>(self);

        return me->frame->getitem(key);
    };

    class_type->fn_str = [](Object *self) -> Object* {
        Module *me = reinterpret_cast<Module *>(self);

        auto s = new (nothrow) Str("Module(" + me->name->data + ")");

        if (!s) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return s;
    };

    class_type->fn_setitem = [](Object *self, Object *key, Object *value) -> Object* {
        Module *me = reinterpret_cast<Module *>(self);

        return me->frame->setitem(key, value);
    };
}

Module *Module::New(const str_t &name) {
    auto namestr = new (nothrow) Str(name);

    if (!namestr) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    auto frame = Frame::New();

    if (!frame)
        return nullptr;

    auto me = new (nothrow) Module(namestr, frame);

    if (!me) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    return me;
}
