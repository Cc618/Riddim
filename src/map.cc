#include "map.hh"
#include "error.hh"

using namespace std;

Type *HashMap::class_type = nullptr;

HashMap::HashMap() : Object(HashMap::class_type) {}

void HashMap::init_class_type() {
    class_type = new (nothrow) Type("HashMap");
    if (!class_type) {
        THROW_MEMORY_ERROR;
        return;
    }

    class_type->fn_traverse_objects = [](Object *self,
                                         const fn_visit_object_t &visit) {
        HashMap *me = reinterpret_cast<HashMap *>(self);

        for (const auto &[k, v] : me->data) {
            visit(v);
            visit(k);
        }
    };
}

Object *HashMap::fn_index(Object *key) {
    // TODO 1 : Use hash objects not pointer value
    auto it = data.find(key);

    if (it == data.end()) {
        // TODO : Use @str function instead of displaying type
        throw_fmt(IndexError, "Key %s not found", key->type->name.c_str());

        return nullptr;
    }

    return (*it).second;
}
