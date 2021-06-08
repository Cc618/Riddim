#pragma once

// TODO C : Update examples + documentation
// Segment tree, used to compute range queries in O(log N) time

#include "object.hh"
#include "methods.hh"
#include "vec.hh"

struct Builtin;

struct SegTree : public DynamicObject {
    static DynamicType *class_type;

    vec_t data;
    Object *functor = nullptr;

    // data are leaves of the segment tree
    static SegTree *New(const vec_t &data, Object *functor);

    // Can throw
    static void init_class_type();
    static void init_class_objects();

protected:
    // data are leaves of the segment tree
    SegTree(const vec_t &data, Object *functor);

private:
    static size_t class_hash;
};
