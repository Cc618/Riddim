#pragma once

// Built in functions

#include "map.hh"
#include "null.hh"
#include "object.hh"
#include "vec.hh"

struct Program;

// --- Macros ---
#define INIT_BUILTIN(NAME, HANDLER, DOC, SIGNATURE)                            \
    auto HANDLER##_obj =                                                       \
        new (nothrow) Builtin(HANDLER, NAME, nullptr, DOC, SIGNATURE);         \
    if (!HANDLER##_obj) {                                                      \
        THROW_MEMORY_ERROR;                                                    \
        return;                                                                \
    }                                                                          \
    auto HANDLER##_name = Str::New(NAME);                                      \
    if (!HANDLER##_name) {                                                     \
        return;                                                                \
    }

#define FAST_INIT_BUILTIN(MODULE, NAME)                                        \
    INIT_BUILTIN(#NAME, builtin_##MODULE##_##NAME, NAME##_doc, NAME##_sig);    \
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
struct Module;

// Handler when the builtins module is loaded
// Note that in comparison to other module handlers,
// this one is called before the riddim module is loaded
void on_builtins_loaded(Module *mod);

// Inits builtins and add them in the program's global frame
void init_builtins();

// --- Functions ---
BUILTIN_HANDLER(builtins, abs);
BUILTIN_HANDLER(builtins, argmax);
BUILTIN_HANDLER(builtins, argmin);
BUILTIN_HANDLER(builtins, argminmax);
BUILTIN_HANDLER(builtins, assert);
BUILTIN_HANDLER(builtins, copy);
BUILTIN_HANDLER(builtins, doc);
BUILTIN_HANDLER(builtins, exit);
BUILTIN_HANDLER(builtins, hash);
BUILTIN_HANDLER(builtins, iter);
BUILTIN_HANDLER(builtins, len);
BUILTIN_HANDLER(builtins, max);
BUILTIN_HANDLER(builtins, min);
BUILTIN_HANDLER(builtins, minmax);
BUILTIN_HANDLER(builtins, next);
BUILTIN_HANDLER(builtins, print);
BUILTIN_HANDLER(builtins, scan);
BUILTIN_HANDLER(builtins, sort);
BUILTIN_HANDLER(builtins, throw);
BUILTIN_HANDLER(builtins, typename);
BUILTIN_HANDLER(builtins, typeof);
