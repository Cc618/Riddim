#pragma once

// Double ended queue type, a queue supporting front / back operations

#include "object.hh"
#include "methods.hh"
#include <deque>

struct Builtin;

// Internal Deque
typedef std::deque<Object*> deque_t;

struct Deque : public DynamicObject {
    static DynamicType *class_type;
    static Deque *empty;

    deque_t data;

    // Methods
    // Push back a new object
    DECL_METHOD(add);

    // Push front
    DECL_METHOD(add_front);

    // Pop back
    DECL_METHOD(pop);

    static Deque *New(const deque_t &data = {});

    // Can throw
    static void init_class_type();
    static void init_class_objects();

protected:
    Deque(const deque_t &data = {});

private:
    static size_t class_hash;
};
