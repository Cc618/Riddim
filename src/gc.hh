#pragma once

// Garbage collection
// Mark and sweep algorithm

struct Object;

struct GcData {
    // Previous / next allocated object
    // nullptr if last / first one
    Object *prev;
    Object *next;

    // The marked flag, if true then we don't delete it
    bool alive = false;
};

// Inits the gc_data field of an object
void init_gc_data(Object *obj);

// Deletes objects not linked to parent
void garbage_collect(Object *parent);
