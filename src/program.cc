#include "program.hh"

Program *Program::instance = nullptr;
Type *Program::class_type = nullptr;

Program::Program(const std::vector<Type *> &types)
    : Object(Program::class_type), types(types) {
    instance = this;
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

        if (program->current_error) visit(program->current_error);
    };
}
