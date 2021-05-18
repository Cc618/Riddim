#pragma once

// Built in functions

#include "map.hh"
#include "null.hh"
#include "object.hh"
#include "vec.hh"

struct Program;

// --- Macros ---
#define INIT_BUILTIN(NAME, HANDLER, DOC, SIGNATURE, GLOBAL)                    \
    auto HANDLER##_obj =                                                       \
        new (nothrow) Builtin(HANDLER, NAME, nullptr, DOC, SIGNATURE);         \
    if (!HANDLER##_obj) {                                                      \
        THROW_MEMORY_ERROR;                                                    \
        return;                                                                \
    }                                                                          \
    auto HANDLER##_name = Str::New(NAME);                                      \
    if (!HANDLER##_name) {                                                     \
        return;                                                                \
    }                                                                          \
    if (GLOBAL && !global_frame->setitem(HANDLER##_name, HANDLER##_obj)) {     \
        return;                                                                \
    }

#define FAST_INIT_BUILTIN(NAME)                                                \
    INIT_BUILTIN(#NAME, builtin_##NAME, NAME##_doc, NAME##_sig, true);

#define FAST_INIT_MODULE_BUILTIN(MODULE, NAME)                                 \
    INIT_BUILTIN(#NAME, builtin_##MODULE##_##NAME, NAME##_doc, NAME##_sig,     \
                 false);                                                       \
    if (!mod->setattr(builtin_##MODULE##_##NAME##_name,                        \
                      builtin_##MODULE##_##NAME##_obj)) {                      \
        return;                                                                \
    }

#define BUILTIN_HANDLER(MODULE, NAME)                                          \
    Object *builtin_##MODULE##_##NAME(Object *self, Object *args,              \
                                      Object *kwargs)

// --- Global Object ---
// Does not have specific data but is global (ex: enditer)
struct Global : public Object {
    static Type *class_type;

    // Can throw
    static void init_class_type();

    Global();
};

// --- Globals ---
extern Global *enditer;

// --- Init ---
// Inits builtins and add them in the program's global frame
void init_builtins();

// --- Functions ---
Object *builtin_assert(Object *self, Object *args, Object *kwargs);

Object *builtin_copy(Object *self, Object *args, Object *kwargs);

Object *builtin_doc(Object *self, Object *args, Object *kwargs);

Object *builtin_exit(Object *self, Object *args, Object *kwargs);

// Throws if is not Int
Object *builtin_hash(Object *self, Object *args, Object *kwargs);

Object *builtin_iter(Object *self, Object *args, Object *kwargs);

Object *builtin_len(Object *self, Object *args, Object *kwargs);

Object *builtin_next(Object *self, Object *args, Object *kwargs);

Object *builtin_print(Object *self, Object *args, Object *kwargs);

Object *builtin_throw(Object *self, Object *args, Object *kwargs);

Object *builtin_typeof(Object *self, Object *args, Object *kwargs);

Object *builtin_typename(Object *self, Object *args, Object *kwargs);

// --- Utils ---
// Prints one object to stdout
// !!! Use it only in debug mode since it is unsafe
inline Object *debug_print(Object *o) {
    return builtin_print(null, Vec::New({o}), HashMap::empty);
}
