#include "gc.hh"
#include "debug.hh"
#include "object.hh"
#include "program.hh"
#include <iostream>
#include <vector>

using namespace std;

// Number of minimum alive objects to execute a garbage collection
constexpr size_t GC_COUNT_THRESHOLD = 1000;

// Number of calls to gc_step to garbage collect if necessary
constexpr size_t GC_PERIOD = 10;

static size_t gc_step_counter = 0;

static Object *last_allocated_object = nullptr;

static size_t gc_object_count = 0;

void init_gc_data(Object *obj) {
    // Insert this object to the gc linked list
    obj->gc_data.prev = last_allocated_object;

    last_allocated_object = obj;

    ++gc_object_count;
}

void garbage_collect(Object *parent) {
#ifdef DEBUG_GC
    debug_info("Collecting garbages, " + to_string(gc_get_count()) +
               " objects");
#endif

    // parent can be nullptr at the destruction of the interpreter
    if (parent) {
        // Mark using an iterative DFS
        vector<Object *> to_mark;
        to_mark.push_back(parent);
        parent->gc_data.alive = true;
        while (!to_mark.empty()) {
            auto obj = to_mark.back();
            to_mark.pop_back();

            // Add all objects that can be accessed from obj
            obj->traverse_objects([&to_mark](Object *child) {
                if (!child)
                    return;

                // Not visited, mark it as alive
                if (!child->gc_data.alive) {
                    child->gc_data.alive = true;
                    to_mark.push_back(child);
                }
            });
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

            --gc_object_count;

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

#ifdef DEBUG_GC
    debug_info("garbage_collect ended, " + to_string(gc_get_count()) +
               " objects");
    cout << endl;
#endif
}

void gc_step() {
    if (gc_step_counter == 0 && gc_object_count >= GC_COUNT_THRESHOLD) {
        garbage_collect(Program::instance);
    }

    ++gc_step_counter;
    gc_step_counter %= GC_PERIOD;
}

size_t gc_get_count() { return gc_object_count; }
