#include "module.hh"
#include "error.hh"
#include "program.hh"

using namespace std;

Type *Module::class_type = nullptr;

Module::Module(Str *name, Code *code, Frame *frame, const str_t &filepath)
    : Object(Module::class_type), name(name), code(code), frame(frame),
      filepath(filepath) {}

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
        visit(me->code);
        visit(me->frame);
    };

    // @getattr
    class_type->fn_getattr = [](Object *self, Object *key) -> Object * {
        Module *me = reinterpret_cast<Module *>(self);

        return me->frame->getitem(key);
    };

    // @str
    class_type->fn_str = [](Object *self) -> Object * {
        Module *me = reinterpret_cast<Module *>(self);

        auto s = new (nothrow) Str("Module(" + me->name->data + ")");

        if (!s) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return s;
    };

    // @setattr
    class_type->fn_setattr = [](Object *self, Object *key,
                                Object *value) -> Object * {
        Module *me = reinterpret_cast<Module *>(self);

        return me->frame->setitem(key, value);
    };
}

Module *Module::New(const str_t &name, const str_t &filepath) {
    auto namestr = new (nothrow) Str(name);

    if (!namestr) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    auto code = Code::New(filepath);

    if (!code)
        return nullptr;

    auto frame = Frame::New(name, filepath, Program::instance->top_frame);

    if (!frame) {
        return nullptr;
    }

    auto me = new (nothrow) Module(namestr, code, frame, filepath);

    if (!me) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    return me;
}
