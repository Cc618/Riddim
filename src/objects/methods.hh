#pragma once

// Method utilities

#include <string>

struct Vec;
struct HashMap;
struct Builtin;

// FN_NAME c string
#define CHECK_ARGS(FN_NAME)                                                    \
    if (args->type != Vec::class_type) {                                       \
        THROW_TYPE_ERROR_PREF((std::string(FN_NAME) + "{args}").c_str(),       \
                              args->type, Vec::class_type);                    \
        return nullptr;                                                        \
    }

// FN_NAME c string
#define CHECK_KWARGS(FN_NAME)                                                  \
    if (kwargs->type != HashMap::class_type) {                                 \
        THROW_TYPE_ERROR_PREF((std::string(FN_NAME) + "{kwargs}").c_str(),     \
                              kwargs->type, HashMap::class_type);              \
        return nullptr;                                                        \
    }

// Defines me, args_data, kwargs_data
// Verify args / kwargs types
// - OBJECT : Struct / class of the object (Object if none)
// - FN, c string : Function / method name
// Returns nullptr on error
#define INIT_METHOD(OBJECT, FN)                                                \
    auto me = reinterpret_cast<OBJECT *>(self);                                \
    { CHECK_ARGS(FN); }                                                        \
    { CHECK_KWARGS(FN); }                                                      \
    auto args_data = reinterpret_cast<Vec *>(args)->data;                      \
    auto kwargs_data = reinterpret_cast<HashMap *>(kwargs)->data;

// Checks that this function contains no arguments
#define CHECK_NOARGS(FN)                                                       \
    if (!args_data.empty()) {                                                  \
        THROW_ARGUMENT_ERROR(FN, "length", "No arguments required");           \
        return nullptr;                                                        \
    }

// Checks that this function contains no kw arguments
#define CHECK_NOKWARGS(FN)                                                     \
    if (!kwargs_data.empty()) {                                                \
        THROW_EXTRA_KWARGS(FN, "No kwargs required");                          \
        return nullptr;                                                        \
    }

// Looks for the number of arguments
// LEN : Int
#define CHECK_ARGSLEN(LEN, FN)                                                 \
    if (args_data.size() != LEN) {                                             \
        THROW_ARGUMENT_ERROR(FN, "length", #LEN " arguments required");        \
        return nullptr;                                                        \
    }

// Inits a method in the factory of a dynamic object (self is the object)
// Call it within init_class_objects
#define NEW_METHOD(TYPE, NAME)                                                 \
    {                                                                          \
        auto method = new Builtin(TYPE::me_##NAME##_handler, #TYPE "." #NAME,  \
                                  TYPE::class_type);                           \
        if (!method) {                                                         \
            THROW_MEMORY_ERROR;                                                \
            return;                                                            \
        }                                                                      \
        TYPE::class_type->attrs[#NAME] = method;                               \
    }

// Declares a new method inside the class of a dynamic object
// (as an attribute)
#define DECL_METHOD(NAME)                                                      \
    static Object *me_##NAME##_handler(Object *self, Object *args,             \
                                       Object *kwargs);
