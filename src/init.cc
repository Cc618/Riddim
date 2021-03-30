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
    // TODO
    // Type::class_type = ...;
    // Object::class_type = ...;
    // Object::class_type = ...;

}

static void init_objects() {
    // TODO : Program...
}

// --- End ---
void end_program() {
    // TODO
    // garbage_collect();
}
