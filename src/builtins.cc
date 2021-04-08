#include "builtins.hh"
#include "error.hh"
#include "map.hh"
#include "null.hh"
#include "str.hh"
#include "vec.hh"
#include <iostream>

using namespace std;

static Object *print_object(Object *o) {
    if (!o) {
        cout << "nullptr";
        return null;
    }

    auto result = o->str();

    // Error
    if (!result) {
        return nullptr;
    }

    if (result->type != Str::class_type) {
        THROW_TYPE_ERROR_PREF("print_object", result->type, Str::class_type);

        return nullptr;
    }

    cout << reinterpret_cast<Str *>(result)->data;

    return null;
}

Object *print(Object *args, Object *kwargs) {
    if (args->type != Vec::class_type) {
        THROW_TYPE_ERROR_PREF("print{args}", args->type, Vec::class_type);

        return nullptr;
    }

    if (kwargs->type != HashMap::class_type) {
        THROW_TYPE_ERROR_PREF("print{kwargs}", kwargs->type,
                              HashMap::class_type);

        return nullptr;
    }

    auto args_data = reinterpret_cast<Vec *>(args)->data;
    auto kwargs_data = reinterpret_cast<HashMap *>(kwargs)->data;

    if (!args_data.empty()) {
        // Dispatch errors
        if (!print_object(args_data[0]))
            return nullptr;

        for (size_t i = 1; i < args_data.size(); ++i) {
            cout << ' ';

            // Dispatch errors
            if (!print_object(args_data[i]))
                return nullptr;
        }
    }

    cout << endl;

    return null;
}
