#include "error.hh"
#include "program.hh"
#include <iostream>
#include <memory>

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

    // Other error types
#define INIT_ERROR(TYPE)                                                       \
    TYPE = new Type(#TYPE);                                                    \
    TYPE->fn_traverse_objects = error_traverse;

    INIT_ERROR(AssertError);

#undef INIT_ERROR
}


#define DECL_ERROR(TYPE) \
    Type *TYPE = nullptr; \
    Error *New##TYPE(error_msg_t msg) { \
        auto error = new Error(msg); \
        error->type = TYPE; \
        return error; \
    }

DECL_ERROR(AssertError);
DECL_ERROR(InternalError);

void throw_error(Object *error) { Program::instance->current_error = error; }

void clear_error() { Program::instance->current_error = nullptr; }

bool is_error(Type *type) {
    return *Program::instance->current_error->type == *type;
}

bool on_error() { return Program::instance->current_error; }

void dump_error() {
    if (!on_error()) return;

    // TODO : Call str()
    cerr << "Error of type " << Program::instance->current_error->type->name << endl;
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
template<typename ... Args>
void throw_fmt(Type *error_type, const char *fmt, Args ... args) {
    // Get size and check errors
    int size = snprintf(nullptr, 0, fmt, args...) + 1;

    if (size <= 0)
        internal_error("throw_fmt : Invalid format");

    // Format
    auto buf = make_unique<char[]>(size);
    snprintf(buf.get(), size, fmt, args...);

    // Throw
    str_t formatted_msg(buf.get(), buf.get() + size - 1);
    throw_str(error_type, formatted_msg);
}

void internal_error(const str_t &msg) {
    cerr << "Fatal error : " << msg << endl;

    exit(-1);
}
