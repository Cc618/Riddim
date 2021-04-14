#include "program.hh"
#include "str.hh"

using namespace std;

Program *Program::instance = nullptr;
Type *Program::class_type = nullptr;

void Program::New(const std::vector<Type *> &types) {
    // We can't handle memory exceptions at this state
    Program::instance = new Program(types);
}

Program::Program(const std::vector<Type *> &types)
    : Object(Program::class_type) {
    instance = this;

    // Register types
    for (auto type : types)
        Program::add_type(type);
}

void Program::init_class_type() {
    class_type = new Type("Program");

    class_type->fn_traverse_objects = [](Object *self,
                                         const fn_visit_object_t &visit) {
        Program *program = reinterpret_cast<Program *>(self);

        // Visit all objects that must be kept alive
        for (auto child : program->types)
            visit(child);

        for (auto child : program->globals)
            visit(child);

        for (auto child : program->modules)
            visit(child);

        for (auto child : program->obj_stack)
            visit(child);

        visit(program->global_frame);
        visit(program->top_frame);
        visit(program->trace);

        if (program->current_error)
            visit(program->current_error);
    };

    // @str
    class_type->fn_str = [](Object *self) -> Object * {
        // TODO : Display modules, types...
        auto result = new (nothrow) Str("Program()");

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };
}

void Program::init_attributes() {
    // Create global frame
    global_frame = Frame::New();

    if (!global_frame) {
        THROW_MEMORY_ERROR;

        return;
    }

    top_frame = global_frame;

    // Register all types that weren't registered yet
    for (auto type : types) {
        register_type(type);

        if (on_error())
            return;
    }
}

void Program::add_type(Type *type) {
    Program::instance->types.push_back(type);

    if (Program::instance->global_frame) {
        Program::instance->register_type(type);

        if (on_error())
            return;
    }
}

void Program::add_module(Module *mod) {
    Program::instance->modules.push_back(mod);
}

void Program::push_frame(Frame *f) {
    f->previous = Program::instance->top_frame;
    Program::instance->top_frame = f;
}

void Program::pop_frame() {
    Program::instance->top_frame = Program::instance->top_frame->previous;
}

void Program::push_trace(Trace *t) {
    t->prev = Program::instance->trace;
    Program::instance->trace = t;
}

void Program::pop_trace() {
    Program::instance->trace = Program::instance->trace->prev;
}

void Program::register_type(Type *type) {
    auto type_id = new (nothrow) Str(type->name);

    if (!type_id) {
        THROW_MEMORY_ERROR;

        return;
    }

    Program::instance->global_frame->setitem(type_id, type);
}
