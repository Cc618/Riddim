#include "bool.hh"
#include "error.hh"
#include "str.hh"
#include "program.hh"
#include "int.hh"

using namespace std;

Type *Bool::class_type = nullptr;

Bool *istrue = nullptr;
Bool *isfalse = nullptr;

Bool *compare(Object *a, Object *b, CmpOp op) {
    auto result = a->cmp(b);

    // Dispatch error
    if (!result) {
        return nullptr;
    }

    if (result->type != Int::class_type) {
        THROW_TYPE_ERROR_PREF("compare", result->type, Int::class_type);

        return nullptr;
    }

    int_t threeway = reinterpret_cast<Int*>(result)->data;
    bool ret;

    switch (op)
    {
    case CmpOp::Equal:
        ret = threeway == 0;
        break;

    case CmpOp::NotEqual:
        ret = threeway == -1;
        break;

    case CmpOp::Lesser:
        ret = threeway < 0;
        break;

    case CmpOp::Greater:
        ret = threeway > 0;
        break;

    case CmpOp::LesserEqual:
        ret = threeway <= 0;
        break;

    case CmpOp::GreaterEqual:
        ret = threeway >= 0;
        break;

    default:
        throw_fmt(InternalError, "compare: Invalid CmpOp %d", op);

        return nullptr;
    }

    return ret ? istrue : isfalse;
}

Bool::Bool(bool data) : Object(Bool::class_type), data(data) {}

void Bool::init_class_type() {
    class_type = new (nothrow) Type("Bool");
    if (!class_type) {
        THROW_MEMORY_ERROR;
        return;
    }

    class_type->fn_str = [](Object *self) -> Object * {
        Bool *me = reinterpret_cast<Bool *>(self);

        auto s = new (nothrow) Str(me->data ? "true" : "false");

        if (!s) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return s;
    };
}

void Bool::init_class_objects() {
    istrue = new (nothrow) Bool(true);

    if (!istrue) {
        THROW_MEMORY_ERROR;

        return;
    }

    isfalse = new (nothrow) Bool(false);

    if (!isfalse) {
        THROW_MEMORY_ERROR;

        return;
    }

    Program::instance->globals.push_back(istrue);
    Program::instance->globals.push_back(isfalse);
}
