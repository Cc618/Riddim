#include "usertype.hh"
#include "bool.hh"
#include "error.hh"
#include "function.hh"
#include "methods.hh"
#include "null.hh"
#include "str.hh"
#include "vec.hh"

using namespace std;

Type *NewUserType(const str_t &name) {
    auto type = DynamicType::New(name);

    if (!type) {
        return nullptr;
    }

    type->constructor = [](Object *self, Object *args,
                                 Object *kwargs) -> Object * {
        INIT_METHOD(DynamicType, reinterpret_cast<DynamicType*>(self)->name);

        auto instance = new (nothrow) DynamicObject(me);

        if (!instance) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        DynamicObject::init(instance);

        if (on_error())
            return nullptr;

        // Check for constructor
        str_t new_key = "@new";
        auto new_it = instance->attrs.find(new_key);
        bool is_constructed = false;

        // @new present
        if (new_it != instance->attrs.end()) {
            auto new_attr = new_it->second;

            if (new_attr != null) {
                // Call the constructor
                auto result = new_attr->call(args, kwargs);

                if (!result) {
                    return nullptr;
                }

                if (result != null) {
                    THROW_TYPE_ERROR_PREF((me->name + ".@new").c_str(),
                                          result->type, Null::class_type);

                    return nullptr;
                }

                is_constructed = true;
            }
        }

        // No args if no constructor
        if (!is_constructed && (!args_data.empty() || !kwargs_data.empty())) {
            THROW_ARGUMENT_ERROR((me->name + ".@new").c_str(), "length",
                                 "No arguments required");

            return nullptr;
        }

        return instance;
    };

    return type;
}
