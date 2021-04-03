#include "object.hh"
#include "error.hh"
#include "hash.hh"
#include "int.hh"
#include "program.hh"
#include "str.hh"
#include <iostream>

using namespace std;

// --- Object ---
Type *Object::class_type = nullptr;

void Object::init_class_type() { class_type = new Type("Object"); }

Object::Object(Type *type) : type(type) {
    init_gc_data(this);

    // TODO : Call new function (of the type) ?
}

Object::~Object() {
    // TODO : Call delete function (of the type) ?
}

void Object::traverse_objects(const fn_visit_object_t &visit) {
    if (type->fn_traverse_objects)
        type->fn_traverse_objects(this, visit);
}

Object *Object::getattr(Object *name) {
    if (!type->fn_getattr) {
        auto name_str = name->str();

        if (!name_str || name_str->type != Str::class_type) {
            clear_error();
            throw_fmt(NameError, "No such attribute for type %s", type);
        } else
            THROW_ATTR_ERROR(type, reinterpret_cast<Str *>(name_str)->data);

        return nullptr;
    }

    return type->fn_getattr(this, name);
}

Object *Object::getitem(Object *args) {
    if (!type->fn_getitem) {
        THROW_NOBUILTIN(getitem);

        return nullptr;
    }

    return type->fn_getitem(this, args);
}

Object *Object::hash() {
    // Default hash using the object memory address and its type
    if (!type->fn_hash) {
        auto result =
            new (nothrow) Int(hash_combine(hash_ptr(this), hash_ptr(type)));

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    }

    return type->fn_hash(this);
}

Object *Object::in(Object *val) {
    if (!type->fn_in) {
        THROW_NOBUILTIN(in);

        return nullptr;
    }

    return type->fn_in(this, val);
}

Object *Object::setattr(Object *name, Object *value) {
    if (!type->fn_setattr) {
        auto name_str = name->str();

        if (!name_str || name_str->type != Str::class_type) {
            clear_error();
            throw_fmt(NameError, "No such attribute for type %s", type);
        } else
            THROW_ATTR_ERROR(type, reinterpret_cast<Str *>(name_str)->data);

        return nullptr;
    }

    return type->fn_setattr(this, name, value);
}

Object *Object::setitem(Object *key, Object *value) {
    if (!type->fn_setitem) {
        THROW_NOBUILTIN(setitem);

        return nullptr;
    }

    return type->fn_setitem(this, key, value);
}

Object *Object::str() {
    // TODO : Default, return Type()
    if (!type->fn_str) {
        auto result = new (nothrow) Str(type->name + "()");

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    }

    return type->fn_str(this);
}

#undef THROW_NOBUILTIN

// --- Type ---
static int type_global_id = 0;

Type *Type::class_type = nullptr;

Type::Type(const str_t &name) : Object(Type::class_type), name(name) {
    // Register this type to the program
    if (Program::instance)
        Program::add_type(this);

    id = ++type_global_id;
}

void Type::init_class_type() {
    class_type = new Type("Type");

    // Was not initialized
    class_type->type = class_type;

    // @str
    class_type->fn_str = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Type *>(self);
        auto result = new (nothrow) Str("Type(" + me->name + ")");

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    };
}

bool Type::operator==(const Type &other) const { return id == other.id; }

// --- TODO ---
#include "debug.hh"
#include "int.hh"
#include "map.hh"
#include "str.hh"
#include "frame.hh"
#include "bool.hh"
#include "module.hh"

void print(Object *o) {
    if (!o) {
        cout << "nullptr" << endl;
        return;
    }

    auto result = o->str();

    // TODO : Errors
    if (!result) {
        cout << "Can't find @str method" << endl;
        return;
    }
    if (result->type != Str::class_type) {
        cout << "Invalid @str type" << endl;
        return;
    }

    // TODO : Type of result error
    cout << reinterpret_cast<Str *>(result)->data << endl;
}

void testObjects() {
    // auto frame = Frame::New();

    // frame->vars->setitem(new Str("a"), new Int(42));
    // frame->vars->setitem(new Str("b"), new Int(618));

    // auto framefn = Frame::New(frame);

    // framefn->vars->setitem(new Str("b"), new Int(1));
    // framefn->vars->setitem(new Str("c"), new Int(2));

    // print(framefn->fetch(new Str("c")));
    // print(framefn->fetch(new Str("b")));
    // print(framefn->fetch(new Str("a")));
    // print(framefn->fetch(new Str("d")));

    auto mod = Module::New("mymod");
    auto frame = mod->frame;
    Program::add_module(mod);
    Program::instance->main_module = mod;

    mod->setitem(new Str("a"), new Int(618));

    // Shouldn't throw errors
    garbage_collect(Program::instance);


    frame->start_lineno = 11;
    frame->code = {
        11, 12, 13, // 11
        21, 22,     // 12

        41,         // 14
        51          // 15
    };

    frame->line_deltas = {
        { 3, 1 },
        { 5, 2 },
        { 6, 1 },
    };

    cout << frame->lineof(0) << endl;
    cout << frame->lineof(2) << endl;
    cout << frame->lineof(3) << endl;
    cout << frame->lineof(frame->code.size() - 2) << endl;
    cout << frame->lineof(frame->code.size() - 1) << endl;
}
