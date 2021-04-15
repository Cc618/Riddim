#include "gc.hh"
#include "object.hh"
#include "program.hh"
#include <vector>

using namespace std;

// Number of minimum alive objects to execute a garbage collection
constexpr size_t gc_count_threshold = 100;

static Object *last_allocated_object = nullptr;

static size_t gc_object_count = 0;

void init_gc_data(Object *obj) {
    // Insert this object to the gc linked list
    obj->gc_data.prev = last_allocated_object;

    last_allocated_object = obj;

    ++gc_object_count;
}

// TODO A
#include "debug.hh"
#include "builtins.hh"
#include "function.hh"
#include "int.hh"
#include <iostream>

void garbage_collect(Object *parent) {
    debug_info("Collecting garbages, " + to_string(gc_get_count()) + " objects");

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

            // TODO
            --gc_object_count;
            // debug_print(node);
            cout << node->type->name << " ";
            if (node->type == Int::class_type) {
                cout << reinterpret_cast<Int*>(node)->data;
            } else if (node->type == Builtin::class_type) {
                cout << reinterpret_cast<Builtin*>(node)->name;
            } else if (node->type == Str::class_type) {
                cout << reinterpret_cast<Str*>(node)->data;
            }

            cout << endl;

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

    debug_info("garbage_collect ended, " + to_string(gc_get_count()) + " objects");
    // TODO
    exit(0);
}

// TODO : Every N instructions
void gc_step() {
    if (gc_object_count >= gc_count_threshold) {
        garbage_collect(Program::instance);
    }
}

size_t gc_get_count() {
    return gc_object_count;
}
