#include "float.hh"
#include "error.hh"
#include "hash.hh"
#include "bool.hh"
#include "int.hh"
#include "map.hh"
#include "str.hh"
#include "vec.hh"

using namespace std;

Type *Float::class_type = nullptr;

Object *Float::zero = nullptr;
Object *Float::one = nullptr;

Float::Float(const float_t &data) : Object(Float::class_type), data(data) {}

void Float::init_class_type() {
    class_type = new (nothrow) Type("Float");

    if (!class_type) {
        THROW_MEMORY_ERROR;

        return;
    }

    // @new
    class_type->constructor = [](Object *self, Object *args,
                                 Object *kwargs) -> Object * {
        INIT_METHOD(Object, "Float");

        CHECK_ARGSLEN(1, "Float");
        CHECK_NOKWARGS("Float");

        float_t data;

        if (args_data[0]->type == Int::class_type) {
            data = reinterpret_cast<Int *>(args_data[0])->data;
        } else if (args_data[0]->type == Float::class_type) {
            data = reinterpret_cast<Float *>(args_data[0])->data;
        } else if (args_data[0]->type == Bool::class_type) {
            data = reinterpret_cast<Bool *>(args_data[0])->data;
        } else if (args_data[0]->type == Str::class_type) {
            auto val = str_to_float(reinterpret_cast<Str *>(args_data[0])->data);

            if (val) {
                data = val.value();
            } else {
                throw_fmt(
                    ArithmeticError,
                    "Float@new{data: Str} : Invalid format");

                return nullptr;
            }
        } else {
            throw_fmt(
                TypeError,
                "Float@new{data} : Data must be either Float, Int or Str");

            return nullptr;
        }

        auto result = new (nothrow) Float(data);

        // Dispatch error
        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @add
    class_type->fn_add = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<Float *>(self);

        if (o->type != Float::class_type) {
            THROW_TYPE_ERROR_PREF("Float.@add", o->type, Float::class_type);

            return nullptr;
        }

        auto result =
            new (nothrow) Float(me->data + reinterpret_cast<Float *>(o)->data);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @cmp
    class_type->fn_cmp = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<Float *>(self);

        int_t res;

        if (o->type != Float::class_type) {
            res = -1;
        } else {
            auto other = reinterpret_cast<Float *>(o)->data;
            res = me->data > other ? 1 : me->data < other ? -1 : 0;
        }

        auto result = new (nothrow) Float(res);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @copy
    class_type->fn_copy = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Float *>(self);

        auto result = new (nothrow) Float(me->data);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @div
    class_type->fn_div = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<Float *>(self);

        if (o->type != Float::class_type) {
            THROW_TYPE_ERROR_PREF("Float.@div", o->type, Float::class_type);

            return nullptr;
        }

        auto odata = reinterpret_cast<Float *>(o)->data;

        if (odata == 0.0) {
            THROW_ARITHMETIC_ERROR("/", "Division by 0");

            return nullptr;
        }

        auto result = new (nothrow) Float(me->data / odata);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @hash
    class_type->fn_hash = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Float *>(self);

        auto hdata = reinterpret_cast<size_t *>(&me->data);

        auto result = new (nothrow) Int(hash_ptr(hdata));

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    /* TODO C : Modulo
    // @mod
    class_type->fn_mod = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<Float *>(self);

        if (o->type != Float::class_type) {
            THROW_TYPE_ERROR_PREF("Float.@mod", o->type, Float::class_type);

            return nullptr;
        }

        auto odata = reinterpret_cast<Float *>(o)->data;

        if (odata == 0) {
            THROW_ARITHMETIC_ERROR("%", "Modulo by 0");

            return nullptr;
        }

        auto result = new (nothrow) Float(me->data % odata);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };
    */

    // @mul
    class_type->fn_mul = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<Float *>(self);

        if (o->type != Float::class_type) {
            THROW_TYPE_ERROR_PREF("Float.@mul", o->type, Float::class_type);

            return nullptr;
        }

        auto result =
            new (nothrow) Float(me->data * reinterpret_cast<Float *>(o)->data);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @neg
    class_type->fn_neg = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Float *>(self);

        auto result = new (nothrow) Float(-me->data);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @str
    class_type->fn_str = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Float *>(self);

        auto result = Str::New(to_string(me->data));

        if (!result) {
            return nullptr;
        }

        return result;
    };

    // @sub
    class_type->fn_sub = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<Float *>(self);

        if (o->type != Float::class_type) {
            THROW_TYPE_ERROR_PREF("Float.@sub", o->type, Float::class_type);

            return nullptr;
        }

        auto result =
            new (nothrow) Float(me->data - reinterpret_cast<Float *>(o)->data);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };
}

void Float::init_class_objects() {
    zero = new (nothrow) Float(0);

    if (!zero) {
        THROW_MEMORY_ERROR;

        return;
    }

    one = new (nothrow) Float(1);

    if (!one) {
        THROW_MEMORY_ERROR;

        return;
    }
}
