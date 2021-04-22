#include "range.hh"
#include "error.hh"
#include "int.hh"
#include "builtins.hh"

using namespace std;

Type *Range::class_type = nullptr;

Range *Range::New(int_t start, int_t end, int_t step, bool inclusive) {
    Range *self = new (nothrow) Range(class_type);

    if (!self) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    // TODO A : Increasing / decreasing check

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

        auto iter = new (nothrow) Iterator([me](Iterator *it) -> Object * {
            int_t &i = reinterpret_cast<int_t&>(it->custom_data);

            if (me->inclusive && i > me->end || !me->inclusive && i >= me->end) return enditer;

            auto result = new (nothrow) Int(i);

            if (!result) {
                THROW_MEMORY_ERROR;

                return nullptr;
            }

            i += me->step;

            return result;
        }, me, reinterpret_cast<void*>(me->start));

        if (!iter) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return iter;
    };
}
