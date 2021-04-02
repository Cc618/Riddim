#include "program.hh"
#include "str.hh"

using namespace std;

Program *Program::instance = nullptr;
Type *Program::class_type = nullptr;

Program::Program(const std::vector<Type *> &types)
    : Object(Program::class_type) {
    instance = this;

    // Register types
    for (auto type : types) Program::add_type(type);
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

        if (program->current_error) visit(program->current_error);
    };

    // @str
    class_type->fn_str = [](Object *self) -> Object* {
        // TODO : Display modules, types...
        auto result = new (nothrow) Str("Program()");

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };
}

void Program::add_type(Type *type) {
    Program::instance->types.push_back(type);
}

void Program::add_module(Module *mod) {
    Program::instance->modules.push_back(mod);
}
