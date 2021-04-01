#include "init.hh"
#include "error.hh"
#include "gc.hh"
#include "int.hh"
#include "map.hh"
#include "null.hh"
#include "object.hh"
#include "program.hh"
#include "str.hh"
#include "frame.hh"

// --- Init ---
// Inits all built in types
static void init_types();

// Inits global objects
static void init_objects();

// Inits the Program instance (a singleton)
static void init_program_instance();

void init_program() {
    init_types();
    init_objects();
}

// Gathers every types declared within init_types
static std::vector<Type *> types;

static void init_types() {
    // Macro to make it simpler and proper
#define INIT_TYPE(TYPE)                                                        \
    TYPE::init_class_type();                                                   \
    types.push_back(TYPE::class_type);

    // TODO : Add functions like fn_str in *::init_class_type

    INIT_TYPE(Type);
    INIT_TYPE(Object);
    INIT_TYPE(Program);
    init_program_instance();

    // Update this macro
    // Now we can raise exceptions
#undef INIT_TYPE
#define INIT_TYPE(TYPE)                                                        \
    TYPE::init_class_type();                                                   \
    if (on_error())                                                            \
        return;

    INIT_TYPE(Error);
    INIT_TYPE(Str);
    INIT_TYPE(Int);
    INIT_TYPE(Null);
    INIT_TYPE(HashMap);
    INIT_TYPE(AttrObject);
    INIT_TYPE(Frame);

#undef INIT_TYPE
}

static void init_program_instance() { Program *program = new Program(types); }

static void init_objects() {
    Null::init_singleton();

    if (on_error()) return;
}

// --- End ---
void end_program() { garbage_collect(Program::instance); }
