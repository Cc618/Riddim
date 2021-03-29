#pragma once

// The base object class

#include "utils.hh"
#include "gc.hh"
#include <vector>

// TODO
void testObjects();

struct Object {
    GcData gc_data;

    // TODO
    std::vector<Object *> children;

    // Can't be deleted by the GC
    static Object *Global();

    // Normal object
    static Object *Local();

    ~Object();

private:
    Object();
};
