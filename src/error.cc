#include "error.hh"

Type *assert_error_type = nullptr;

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
#define INIT_ERROR(TYPE, TYPEVAR) \
    TYPEVAR = new Type(#TYPE); \
    TYPEVAR->fn_traverse_objects = error_traverse;

    INIT_ERROR(AssertError, assert_error_type);

#undef INIT_ERROR
}

Error *AssertError(error_msg_t msg) {
    auto error = new Error(msg);
    error->type = assert_error_type;

    return error;
}
