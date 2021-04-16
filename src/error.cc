#include "error.hh"
#include "debug.hh"
#include "program.hh"
#include "str.hh"
#include <cstdarg>
#include <iostream>
#include <memory>

using namespace std;

// Dumps this error on out
// This function cannot throw another error
void dump_error_object(Object *error, ostream &out);

Type *Error::class_type = nullptr;

Error::Error(error_msg_t msg) : Object(Error::class_type), msg(msg) {}

void Error::init_class_type() {
    auto error_traverse_objects = [](Object *self,
                                     const fn_visit_object_t &visit) {
        Error *obj = reinterpret_cast<Error *>(self);
    };

    // @getattr
    auto error_getattr = [](Object *self, Object *name) -> Object * {
        Error *me = reinterpret_cast<Error *>(self);

        if (name->type != Str::class_type) {
            THROW_TYPE_ERROR_PREF("@getattr", name->type, Str::class_type);

            return nullptr;
        }

        auto attr = reinterpret_cast<Str *>(name)->data;

        if (attr == "msg") {
            auto result = new (nothrow) Str(me->msg);

            if (!result) {
                THROW_MEMORY_ERROR;

                return nullptr;
            }

            return result;
        }

        THROW_ATTR_ERROR(me->type, attr);

        return nullptr;
    };

    // @str
    auto error_str = [](Object *self) -> Object * {
        Error *me = reinterpret_cast<Error *>(self);

        auto result = new (nothrow)
            Str(C_RED + me->type->name + C_NORMAL + "(" + me->msg + ")");

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

#define ERROR_CONSTRUCTOR(TYPE)                                                \
    [](Object *self, Object *args, Object *kwargs) -> Object * {               \
        INIT_METHOD(Object, #TYPE);                                            \
        CHECK_ARGSLEN(1, #TYPE);                                               \
        CHECK_NOKWARGS(#TYPE);                                                 \
        if (args_data[0]->type != Str::class_type) {                           \
            THROW_TYPE_ERROR_PREF(#TYPE "{msg}", args_data[0]->type,           \
                                  Str::class_type);                            \
            return nullptr;                                                    \
        }                                                                      \
        auto result = New##TYPE(reinterpret_cast<Str *>(args_data[0])->data);  \
        if (!result) {                                                         \
            return nullptr;                                                    \
        }                                                                      \
        return result;                                                         \
    };

#define SETUP_ERROR(TYPE)                                                      \
    TYPE->constructor = ERROR_CONSTRUCTOR(TYPE);                               \
    TYPE->fn_traverse_objects = error_traverse_objects;                        \
    TYPE->fn_str = error_str;                                                  \
    TYPE->fn_getattr = error_getattr;

    // Default type
    class_type = new Type("Error");
    auto &Error = class_type;
    SETUP_ERROR(Error);

    MemoryError = new Type("MemoryError");
    SETUP_ERROR(MemoryError);

    // Now, we can throw memory errors
    // Other error types
#define INIT_ERROR(TYPE)                                                       \
    TYPE = new (nothrow) Type(#TYPE);                                          \
    if (!TYPE) {                                                               \
        THROW_MEMORY_ERROR;                                                    \
        return;                                                                \
    }                                                                          \
    SETUP_ERROR(TYPE)

    INIT_ERROR(ArgumentError);
    INIT_ERROR(ArithmeticError);
    INIT_ERROR(AssertError);
    INIT_ERROR(ImportError);
    INIT_ERROR(IndexError);
    INIT_ERROR(InternalError);
    INIT_ERROR(IterError);
    INIT_ERROR(NameError);
    INIT_ERROR(NotImplementedError);
    INIT_ERROR(RecursionError);
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

// Basic error, cannot specify type
Error *NewError(error_msg_t msg) {
    auto error = new (nothrow) Error(msg);

    if (!error) {
        THROW_MEMORY_ERROR;
        return nullptr;
    }

    return error;
}

// Can return nullptr on throw
DECL_ERROR(ArgumentError);
DECL_ERROR(ArithmeticError);
DECL_ERROR(AssertError);
DECL_ERROR(ImportError);
DECL_ERROR(IndexError);
DECL_ERROR(InternalError);
DECL_ERROR(IterError);
DECL_ERROR(MemoryError);
DECL_ERROR(NameError);
DECL_ERROR(NotImplementedError);
DECL_ERROR(RecursionError);
DECL_ERROR(TypeError);

#undef DECL_ERROR

void throw_error(Object *error) {
#ifdef DEBUG_ERRORS
    debug_err("New thrown error");
    dump_error_object(error, cerr);
#endif

    Program::instance->current_error = error;
}

void clear_error() { Program::instance->current_error = nullptr; }

bool is_error(Type *type) {
    return *Program::instance->current_error->type == *type;
}

bool on_error() { return Program::instance->current_error; }

void dump_error() {
    if (!on_error())
        return;

    dump_error_object(Program::instance->current_error, cerr);
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

void dump_error_object(Object *error, ostream &out) {

    if (!error)
        out << "nullptr" << endl;
    else {
        // Borrow the current error
        auto current_error = Program::instance->current_error;
        Program::instance->current_error = nullptr;

        auto msg = error->str();

        // If another error happened
        if (!msg || msg->type != Str::class_type)
            out << error->class_type->name << "()" << endl;
        else
            out << reinterpret_cast<Str *>(msg)->data << endl;

        // Set again this error
        Program::instance->current_error = current_error;
    }
}

CodeGenException::CodeGenException(const std::string &msg,
                                   const std::string &filename, int lineno) {
    rawmsg = filename + ":" + to_string(lineno) + " : " + msg;
}
const char *CodeGenException::what() const noexcept { return rawmsg.c_str(); }
