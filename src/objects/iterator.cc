#include "iterator.hh"
#include "error.hh"
#include "int.hh"
#include "builtins.hh"

using namespace std;

// --- Iterator ---
Type *Iterator::class_type = nullptr;

Iterator::Iterator(const fn_iterator_next_item_t &next_item, Object *collection,
                   void *custom_data, const fn_iterator_delete_item_t &delete_custom_data)
    : Object(Iterator::class_type), next_item(next_item),
      collection(collection), custom_data(custom_data), delete_custom_data(delete_custom_data) {}

Iterator::~Iterator() {
    if (delete_custom_data) delete_custom_data(this);
}

void Iterator::init_class_type() {
    class_type = new (nothrow) Type("Iterator");

    if (!class_type) {
        THROW_MEMORY_ERROR;

        return;
    }

    class_type->fn_traverse_objects = [](Object *self,
                                         const fn_visit_object_t &visit) {
        Iterator *me = reinterpret_cast<Iterator *>(self);

        visit(me->collection);
    };

    // @next
    class_type->fn_next = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Iterator *>(self);

        auto result = me->next_item(me);

        if (!result) {
            return nullptr;
        }

        return result;
    };
}
