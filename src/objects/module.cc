#include "module.hh"
#include "doc.hh"
#include "error.hh"
#include "null.hh"
#include "program.hh"

using namespace std;

void merge_frames(Frame *base, Frame *merged) {
    for (const auto &[h, kv] : merged->vars->data) {
        const auto &[name, value] = kv;

        if (name->type != Str::class_type) {
            continue;
        }

        auto name_str = reinterpret_cast<Str *>(name)->data;

        // No special names
        if (name_str.empty() || name_str == "mod" || name_str[0] == '!' ||
            name_str[0] == '@') {
            continue;
        }

        // Merge this pair
        base->vars->data[h] = kv;
    }
}

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

    // @doc
    class_type->fn_doc = [](Object *self) -> Object * {
        auto me = dynamic_cast<Module *>(self);

        if (!me) {
            throw_fmt(RuntimeError, "Module.@doc got an invalid self instance");

            return nullptr;
        }

        str_t current_doc;

        // The !doc attribute may contain the documentation
        auto docattr = Str::New("!doc");
        if (!docattr) {
            return nullptr;
        }

        auto it = me->frame->fetch(docattr);
        if (it) {
            if (it->type != Str::class_type) {
                THROW_TYPE_ERROR_PREF("Module!doc", it->type, Str::class_type);

                return nullptr;
            }

            current_doc = reinterpret_cast<Str *>(it)->data;
        } else {
            clear_error();
        }

        // Filter what to document
        vector<pair<str_t, Object *>> children;
        for (const auto &[h, kv] : me->frame->vars->data) {
            const auto &[child_name, child] = kv;

            if (child_name->type != Str::class_type) {
                continue;
            }

            auto child_name_str = reinterpret_cast<Str *>(child_name)->data;

            if (child_name_str.size() && child_name_str[0] != '@' &&
                child_name_str[0] != '!' && child_name_str != "mod") {
                children.push_back({"", child});
            }
        }

        str_t result = autodoc(1, me->name->data, current_doc, children);

        if (on_error()) {
            return nullptr;
        }

        if (result.empty()) {
            return null;
        }

        auto sresult = Str::New(result);

        if (!sresult) {
            return nullptr;
        }

        return sresult;
    };

    // @getattr
    class_type->fn_getattr = [](Object *self, Object *key) -> Object * {
        Module *me = reinterpret_cast<Module *>(self);

        return me->frame->getitem(key);
    };

    // @str
    class_type->fn_str = [](Object *self) -> Object * {
        Module *me = reinterpret_cast<Module *>(self);

        auto s = Str::New("Module(" + me->name->data + ")");

        if (!s) {
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
    auto namestr = Str::New(name);

    if (!namestr) {
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
