#include "init.hh"
#include "gc.hh"
#include "object.hh"
#include "program.hh"

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
static std::vector<Type *> types;

static void init_types() {
    // TODO : Add functions like fn_str

    // Type
    Type::init_class_type();
    types.push_back(Type::class_type);

    // Object
    Object::init_class_type();
    types.push_back(Object::class_type);

    // Program
    Program::init_class_type();
    types.push_back(Program::class_type);
}

static void init_objects() { Program *program = new Program(types); }

// --- End ---
void end_program() { garbage_collect(Program::instance); }
