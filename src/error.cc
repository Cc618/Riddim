#include "error.hh"
#include "program.hh"
#include <iostream>
#include <memory>
#include <cstdarg>

using namespace std;

Type *Error::class_type = nullptr;

Error::Error(error_msg_t msg) : Object(Error::class_type), msg(msg) {}

void Error::init_class_type() {
    auto error_traverse = [](Object *self, const fn_visit_object_t &visit) {
        Error *obj = reinterpret_cast<Error *>(self);

        // TODO : Visit msg
    };

    // Default type
    class_type = new Type("Error");
    class_type->fn_traverse_objects = error_traverse;

    MemoryError = new Type("MemoryError");
    MemoryError->fn_traverse_objects = error_traverse;

    // Now, we can throw memory errors
    // Other error types
#define INIT_ERROR(TYPE)                                                       \
    TYPE = new (nothrow) Type(#TYPE);                                          \
    if (!TYPE) {                                                               \
        THROW_MEMORY_ERROR;                                                    \
        return;                                                                \
    }                                                                          \
    TYPE->fn_traverse_objects = error_traverse;

    INIT_ERROR(ArithmeticError);
    INIT_ERROR(AssertError);
    INIT_ERROR(ImportError);
    INIT_ERROR(IndexError);
    INIT_ERROR(InternalError);
    INIT_ERROR(IterError);
    INIT_ERROR(NameError);
    INIT_ERROR(NotImplementedError);
    INIT_ERROR(NullError);
    INIT_ERROR(TypeError);

#undef INIT_ERROR
}

#define DECL_ERROR(TYPE)                                                       \
    Type *TYPE = nullptr;                                                      \
    Error *New##TYPE(error_msg_t msg) {                                        \
        auto error = new (nothrow) Error(msg);                                 \
        if (!error) {                                                          \
            THROW_MEMORY_ERROR;                                                \
            return nullptr;                                                    \
        }                                                                      \
        error->type = TYPE;                                                    \
        return error;                                                          \
    }

// Can return nullptr on throw
DECL_ERROR(ArithmeticError);
DECL_ERROR(AssertError);
DECL_ERROR(ImportError);
DECL_ERROR(IndexError);
DECL_ERROR(InternalError);
DECL_ERROR(IterError);
DECL_ERROR(MemoryError);
DECL_ERROR(NameError);
DECL_ERROR(NotImplementedError);
DECL_ERROR(NullError);
DECL_ERROR(TypeError);

#undef DECL_ERROR

void throw_error(Object *error) { Program::instance->current_error = error; }

void clear_error() { Program::instance->current_error = nullptr; }

bool is_error(Type *type) {
    return *Program::instance->current_error->type == *type;
}

bool on_error() { return Program::instance->current_error; }

void dump_error() {
    if (!on_error())
        return;

    // TODO : Call str()
    cerr << "Error of type " << Program::instance->current_error->type->name
         << endl;
}

bool err_assert(bool assertion, const str_t &msg) {
    if (!assertion) {
        throw_error(NewAssertError(msg));

        return true;
    }

    return false;
}

void throw_str(Type *error_type, const str_t &msg) {
    auto error = new Error(msg);
    error->type = error_type;

    throw_error(error);
}

// !!! Don't forget to return from the function
void throw_fmt(Type *error_type, const char *fmt, ...) {
    va_list arg_list;

    // Get size and check errors
    va_start(arg_list, fmt);
    int size = vsnprintf(nullptr, 0, fmt, arg_list) + 1;
    va_end(arg_list);

    if (size <= 0)
        internal_error("throw_fmt : Invalid format");

    // Format
    auto buf = make_unique<char[]>(size);
    va_start(arg_list, fmt);
    vsnprintf(buf.get(), size, fmt, arg_list);
    va_end(arg_list);

    // Throw
    str_t formatted_msg(buf.get(), buf.get() + size - 1);
    throw_str(error_type, formatted_msg);
}

void internal_error(const str_t &msg) {
    cerr << "Fatal error : " << msg << endl;

    exit(-1);
}
