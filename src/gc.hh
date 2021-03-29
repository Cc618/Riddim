#pragma once

// Garbage collection
// Mark and sweep algorithm

struct Object;

struct GcData {
    // Previous allocated object, nullptr if first one
    // Singly linked list
    Object *prev = nullptr;

    // The marked flag, if true then we don't delete it
    bool alive = false;
};

// Inits the gc_data field of an object
void init_gc_data(Object *obj);

// Deletes objects not linked to parent
void garbage_collect(Object *parent);
