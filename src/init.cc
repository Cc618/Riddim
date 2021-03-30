#include "init.hh"
#include "object.hh"
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

static void init_types() {
    // TODO : Add functions like fn_str

    // Type
    Type::class_type = new Type("Type");
    // Was not initialized
    Type::class_type->type = Type::class_type;

    // Object
    Object::class_type = new Type("Object");
}

static void init_objects() {
    // TODO : Program...
}

// --- End ---
void end_program() {
    // TODO
    // garbage_collect();
}
