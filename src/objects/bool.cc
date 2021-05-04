#include "bool.hh"
#include "error.hh"
#include "int.hh"
#include "program.hh"
#include "str.hh"

using namespace std;

Type *Bool::class_type = nullptr;

Bool *istrue = nullptr;
Bool *isfalse = nullptr;

static string bool_cmpop_str(CmpOp op) {
    switch (op) {
    case CmpOp::Equal:
        return "==";

    case CmpOp::NotEqual:
        return "!=";

    case CmpOp::Lesser:
        return "<";

    case CmpOp::Greater:
        return ">";

    case CmpOp::LesserEqual:
        return "<=";

    case CmpOp::GreaterEqual:
        return ">=";
    }

    return "<unknown CmpOp>";
}

static string bool_binop_str(BoolBinOp op) {
    switch (op) {
    case BoolBinOp::And:
        return "and";

    case BoolBinOp::Or:
        return "or";
    }

    return "<unknown BoolBinOp>";
}

Bool *compare(Object *a, Object *b, CmpOp op) {
    auto result = a->cmp(b);

    // Dispatch error
    if (!result) {
        return nullptr;
    }

    if (result->type != Int::class_type) {
        THROW_TYPE_ERROR_PREF(bool_cmpop_str(op).c_str(), result->type, Int::class_type);

        return nullptr;
    }

    int_t threeway = reinterpret_cast<Int *>(result)->data;
    bool ret;

    switch (op) {
    case CmpOp::Equal:
        ret = threeway == 0;
        break;

    case CmpOp::NotEqual:
        ret = threeway != 0;
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

Bool *bool_binop(Object *a, Object *b, BoolBinOp op) {
    if (a->type != Bool::class_type) {
        THROW_TYPE_ERROR_PREF(bool_binop_str(op).c_str(), a->type, Bool::class_type);

        return nullptr;
    }

    if (b->type != Bool::class_type) {
        THROW_TYPE_ERROR_PREF(bool_binop_str(op).c_str(), b->type, Bool::class_type);

        return nullptr;
    }

    bool result;
    bool aval = reinterpret_cast<Bool *>(a)->data;
    bool bval = reinterpret_cast<Bool *>(b)->data;

    switch (op) {
    case BoolBinOp::And:
        result = aval && bval;
        break;

    case BoolBinOp::Or:
        result = aval || bval;
        break;

    default:
        throw_fmt(InternalError, "bool_binop: Invalid BoolBinOp %d", op);

        return nullptr;
    }

    return result ? istrue : isfalse;
}

Bool *bool_not(Object *o) {
    if (o->type != Bool::class_type) {
        THROW_TYPE_ERROR_PREF("not", o->type, Bool::class_type);

        return nullptr;
    }

    return reinterpret_cast<Bool *>(o)->data ? isfalse : istrue;
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

        auto s = Str::New(me->data ? "true" : "false");

        if (!s) {
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
}
