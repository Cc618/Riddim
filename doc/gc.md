# Garbage Collector / Memory
The garbage collector (GC) is used to remove unreferenced objects.

## Algorithm
The algorithm is the Mark & Sweep algorithm.
This deletes all objects that are unreachable from the root object,
which is Program::instance.

1. Mark every objects reachable from the root using Type::fn_traverse_objects.
2. Delete every objects not marked and reset living objects.

## Type::fn_traverse_objects
This function on the C++ API is used to declare all objects (children) referenced by this object.
For instance, a collection will reference every item in it (keys / values) and also its methods.

## Files
src/gc.{cc,hh}
