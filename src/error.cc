#include "error.hh"
#include "program.hh"

Type *AssertError = nullptr;

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
#define INIT_ERROR(TYPE) \
    TYPE = new Type(#TYPE); \
    TYPE->fn_traverse_objects = error_traverse;

    INIT_ERROR(AssertError);

#undef INIT_ERROR
}

Error *NewAssertError(error_msg_t msg) {
    auto error = new Error(msg);
    error->type = AssertError;

    return error;
}

void throw_error(Object *error) {
    Program::instance->current_error = error;
}

void clear_error() {
    Program::instance->current_error = nullptr;
}

bool is_error(Type *type) {
    return *Program::instance->current_error->type == *type;
}

bool on_error() {
    return Program::instance->current_error;
}

bool err_assert(bool assertion, const str_t &msg) {
    if (!assertion) {
        throw_error(NewAssertError(msg));

        return true;
    }

    return false;
}