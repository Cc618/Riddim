#include "range.hh"
#include "builtins.hh"
#include "error.hh"
#include "int.hh"

using namespace std;

Type *Range::class_type = nullptr;

Range *Range::New(int_t start, int_t end, int_t step, bool inclusive) {
    // Infinite loop test
    if (step == 0 && (inclusive || start != end)) {
        throw_fmt(RuntimeError, "Infinite %sRange%s, step is %s0%s", C_GREEN,
                  C_NORMAL, C_BLUE, C_NORMAL);

        return nullptr;
    }

    if (step < 0 && start < end) {
        throw_fmt(RuntimeError,
                  "Infinite %sRange%s, step is < %s0%s and start < end",
                  C_GREEN, C_NORMAL, C_BLUE, C_NORMAL);

        return nullptr;
    }

    if (step > 0 && start > end) {
        throw_fmt(RuntimeError,
                  "Infinite %sRange%s, step is > %s0%s and start > end",
                  C_GREEN, C_NORMAL, C_BLUE, C_NORMAL);

        return nullptr;
    }

    Range *self = new (nothrow) Range(class_type);

    if (!self) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    self->start = start;
    self->end = end;
    self->step = step;
    self->inclusive = inclusive;

    return self;
}

void Range::init_class_type() {
    class_type = new (nothrow) Type("Range");

    if (!class_type) {
        THROW_MEMORY_ERROR;

        return;
    }

    // @iter
    class_type->fn_iter = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Range *>(self);

        auto iter = new (nothrow) Iterator(
            [me](Iterator *it) -> Object * {
                int_t &i = reinterpret_cast<int_t &>(it->custom_data);

                if (me->step > 0) {
                    if (me->inclusive && i > me->end ||
                        !me->inclusive && i >= me->end)
                        return enditer;
                } else {
                    if (me->inclusive && i < me->end ||
                        !me->inclusive && i <= me->end)
                        return enditer;
                }

                auto result = new (nothrow) Int(i);

                if (!result) {
                    THROW_MEMORY_ERROR;

                    return nullptr;
                }

                i += me->step;

                return result;
            },
            me, reinterpret_cast<void *>(me->start));

        if (!iter) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return iter;
    };
}
