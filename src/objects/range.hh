#pragma once

// Range iterator object

#include "iterator.hh"

struct Range : public Object {
    static Type *class_type;

    int_t start;
    int_t end;
    int_t step;
    bool inclusive;

    static Range *New(int_t start, int_t end, int_t step, bool inclusive);

    // Can throw
    static void init_class_type();

protected:
    using Object::Object;
};
