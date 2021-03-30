#include "init.hh"
#include "object.hh"
#include "program.hh"
#include "gc.hh"

// --- Init ---
// Inits all built in types
static void init_types();

// Inits global objects
static void init_objects();

void init_program() {
    init_types();
    init_objects();
}

// Gathers every types declared within init_types
static std::vector<Type*> types;

static void init_types() {
    // TODO : Add functions like fn_str

    // Type
    Type::class_type = new Type("Type");
    // Was not initialized
    Type::class_type->type = Type::class_type;

    types.push_back(Type::class_type);

    // Object
    Object::class_type = new Type("Object");
    types.push_back(Object::class_type);

    // Program
    Program::init_class_type();
    types.push_back(Program::class_type);
}

static void init_objects() {
    Program *program = new Program(types);
}

// --- End ---
void end_program() {
    garbage_collect(Program::instance);
}
