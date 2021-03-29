#include "gc.hh"
#include "object.hh"
#include <vector>
// TODO
#include <iostream>

using namespace std;

// TODO : When GC is done, update it to the first object ?
static Object *last_allocated_object = nullptr;

void init_gc_data(Object *obj) {
    // Insert this object to the gc linked list
    obj->gc_data = GcData{last_allocated_object};

    last_allocated_object = obj;
}

void garbage_collect(Object *parent) {
    // Mark using an iterative DFS
    vector<Object *> to_mark;
    to_mark.push_back(parent);
    parent->gc_data.alive = true;
    while (!to_mark.empty()) {
        auto obj = to_mark.back();
        to_mark.pop_back();

        // Add all objects that can be accessed from obj
        for (auto child : obj->children)
            if (!child->gc_data.alive) {
                child->gc_data.alive = true;
                to_mark.push_back(child);
            }
    }

    // Sweep by iterating through the gc list
    Object *node = last_allocated_object;

    // To set last_allocated_object to the last alive object
    Object *last_alive_object = nullptr;

    // Next alive node
    Object *next_node = nullptr;

    while (node) {
        auto prev_node = node->gc_data.prev;

        // Not alive, delete it
        if (!node->gc_data.alive) {
            // Remove it from the linked list
            if (next_node)
                next_node->gc_data.prev = prev_node;

            delete node;
        } else {
            // This object is alive
            if (!last_alive_object)
                last_alive_object = node;

            // Reset flags
            node->gc_data.alive = false;

            // This is the new next node
            next_node = node;
        }

        // Next iteration
        node = prev_node;
    }

    // Update last allocated object
    last_allocated_object = last_alive_object;
}
