#include "int.hh"
#include "bool.hh"
#include "error.hh"
#include "float.hh"
#include "hash.hh"
#include "map.hh"
#include "str.hh"
#include "vec.hh"

using namespace std;

int_t get_mod_index(int_t i, int_t len) {
    if (i >= 0)
        return i;
    if (-i > len)
        return i;

    return len + i;
}

Type *Int::class_type = nullptr;

Object *Int::zero = nullptr;
Object *Int::one = nullptr;

Int::Int(const int_t &data) : Object(Int::class_type), data(data) {}

void Int::init_class_type() {
    class_type = new (nothrow) Type("Int");

    if (!class_type) {
        THROW_MEMORY_ERROR;

        return;
    }

    // @new
    class_type->constructor = [](Object *self, Object *args,
                                 Object *kwargs) -> Object * {
        INIT_METHOD(Object, "Int");

        CHECK_ARGSLEN(1, "Int");
        CHECK_NOKWARGS("Int");

        int_t data;

        if (args_data[0]->type == Int::class_type) {
            data = reinterpret_cast<Int *>(args_data[0])->data;
        } else if (args_data[0]->type == Float::class_type) {
            data = reinterpret_cast<Float *>(args_data[0])->data;
        } else if (args_data[0]->type == Bool::class_type) {
            data = reinterpret_cast<Bool *>(args_data[0])->data;
        } else if (args_data[0]->type == Str::class_type) {
            auto val = str_to_int(reinterpret_cast<Str *>(args_data[0])->data);

            if (val) {
                data = val.value();
            } else {
                throw_fmt(ArithmeticError,
                          "Int@new{data: Str} : Invalid format");

                return nullptr;
            }
        } else {
            throw_fmt(TypeError,
                      "Int@new{data} : Data must be either Int, Float or Str");

            return nullptr;
        }

        auto result = new (nothrow) Int(data);

        // Dispatch error
        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @add
    class_type->fn_add = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<Int *>(self);

        // Cast to float
        if (o->type == Float::class_type) {
            auto me_float = new (nothrow) Float(me->data);

            if (!me_float) {
                THROW_MEMORY_ERROR;

                return nullptr;
            }

            return me_float->add(o);
        }

        if (o->type != Int::class_type) {
            THROW_TYPE_ERROR_PREF("Int.@add", o->type, Int::class_type);

            return nullptr;
        }

        auto result =
            new (nothrow) Int(me->data + reinterpret_cast<Int *>(o)->data);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @cmp
    class_type->fn_cmp = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<Int *>(self);

        // Cast to float
        if (o->type == Float::class_type) {
            auto me_float = new (nothrow) Float(me->data);

            if (!me_float) {
                THROW_MEMORY_ERROR;

                return nullptr;
            }

            return me_float->cmp(o);
        }

        int_t res;

        if (o->type != Int::class_type) {
            res = -1;
        } else {
            auto other = reinterpret_cast<Int *>(o)->data;
            res = me->data > other ? 1 : me->data < other ? -1 : 0;
        }

        auto result = new (nothrow) Int(res);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @copy
    class_type->fn_copy = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Int *>(self);

        auto result = new (nothrow) Int(me->data);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // Always casts to float
    // @div
    class_type->fn_div = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<Int *>(self);

        // Cast to float
        auto me_float = new (nothrow) Float(me->data);

        if (!me_float) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return me_float->div(o);
    };

    // @hash
    class_type->fn_hash = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Int *>(self);

        auto result = new (nothrow) Int(hash_int(me->data));

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @mod
    class_type->fn_mod = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<Int *>(self);

        // Cast to float
        if (o->type == Float::class_type) {
            auto me_float = new (nothrow) Float(me->data);

            if (!me_float) {
                THROW_MEMORY_ERROR;

                return nullptr;
            }

            return me_float->mod(o);
        }

        if (o->type != Int::class_type) {
            THROW_TYPE_ERROR_PREF("Int.@mod", o->type, Int::class_type);

            return nullptr;
        }

        auto odata = reinterpret_cast<Int *>(o)->data;

        if (odata == 0) {
            THROW_ARITHMETIC_ERROR("%", "Modulo by 0");

            return nullptr;
        }

        auto result = new (nothrow) Int(me->data % odata);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @mul
    class_type->fn_mul = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<Int *>(self);

        // Cast to float
        if (o->type == Float::class_type) {
            auto me_float = new (nothrow) Float(me->data);

            if (!me_float) {
                THROW_MEMORY_ERROR;

                return nullptr;
            }

            return me_float->mul(o);
        }

        if (o->type != Int::class_type) {
            THROW_TYPE_ERROR_PREF("Int.@mul", o->type, Int::class_type);

            return nullptr;
        }

        auto result =
            new (nothrow) Int(me->data * reinterpret_cast<Int *>(o)->data);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @neg
    class_type->fn_neg = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Int *>(self);

        auto result = new (nothrow) Int(-me->data);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };

    // @str
    class_type->fn_str = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Int *>(self);

        // TODO : Unicode
        auto result = Str::New(to_string(me->data));

        if (!result) {
            return nullptr;
        }

        return result;
    };

    // @sub
    class_type->fn_sub = [](Object *self, Object *o) -> Object * {
        auto me = reinterpret_cast<Int *>(self);

        // Cast to float
        if (o->type == Float::class_type) {
            auto me_float = new (nothrow) Float(me->data);

            if (!me_float) {
                THROW_MEMORY_ERROR;

                return nullptr;
            }

            return me_float->sub(o);
        }

        if (o->type != Int::class_type) {
            THROW_TYPE_ERROR_PREF("Int.@sub", o->type, Int::class_type);

            return nullptr;
        }

        auto result =
            new (nothrow) Int(me->data - reinterpret_cast<Int *>(o)->data);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };
}

void Int::init_class_objects() {
    zero = new (nothrow) Int(0);

    if (!zero) {
        THROW_MEMORY_ERROR;

        return;
    }

    one = new (nothrow) Int(1);

    if (!one) {
        THROW_MEMORY_ERROR;

        return;
    }
}
