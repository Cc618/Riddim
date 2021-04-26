#pragma once

// Garbage collection
// Mark and sweep algorithm

#include <cstddef>

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

// Called every instruction
// This collect garbages once a max objects alive threshold is reached
// The root of the GC is Program
void gc_step();

// Returns the number of alive objects
// O(1)
size_t gc_get_count();
