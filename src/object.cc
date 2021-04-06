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

Object *Object::add(Object *o) {
    if (!type->fn_add) {
        THROW_NOBUILTIN(add);

        return nullptr;
    }

    return type->fn_add(this, o);
}

Object *Object::call(Object *args, Object *kwargs) {
    if (!type->fn_call) {
        THROW_NOBUILTIN(call);

        return nullptr;
    }

    return type->fn_call(this, args, kwargs);
}

Object *Object::cmp(Object *o) {
    if (!type->fn_cmp) {
        bool iseq = this == o;

        auto result = new (nothrow) Int(iseq ? 0 : -1);

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    }

    return type->fn_cmp(this, o);
}

Object *Object::copy() {
    if (!type->fn_copy) {
        return this;
    }

    return type->fn_copy(this);
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

Object *Object::mul(Object *o) {
    if (!type->fn_mul) {
        THROW_NOBUILTIN(mul);

        return nullptr;
    }

    return type->fn_mul(this, o);
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
#include "vec.hh"
#include "interpreter.hh"
#include "null.hh"
#include "function.hh"

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

using namespace OpCode;

void testObjects() {
    // --- Test ---

    // --- Init ---
    // > Function
    // auto fn = new Function([](Object *thisfn, Object *args, Object *kwargs) -> Object* {
    //     print(args);
    //     print(kwargs);

    //     return new Int(42);
    // });

    // auto kw = new HashMap();
    // kw->setitem(new Str("kw1"), new Str("v1"));
    // kw->setitem(new Str("kw2"), new Str("v2"));

    // auto args = new Vec({new Int(1), new Int(2)});

    // TODO : Test with previous frame
    auto frame = Frame::New();

    // --- Init Vars ---
    auto zero = frame->add_const(new Int(0));
    auto one = frame->add_const(new Int(1));
    auto otrue = frame->add_const(istrue);
    auto ofalse = frame->add_const(isfalse);
    auto fnull = frame->add_const(null);
    auto tnull = frame->add_const(Null::class_type);
    auto a = frame->add_const(new Str("Hello"));
    auto aname = frame->add_const(new Str("a"));
    auto ok = frame->add_const(new Str("OK"));
    auto nok = frame->add_const(new Str("Not OK"));
    // auto myfn = frame->add_const(fn);
    // auto myfnargs = frame->add_const(args);
    // auto myfnkw = frame->add_const(kw);

    // --- Code ---
    frame->code = {
        LoadConst, otrue,
        LoadConst, ofalse,
        BinBool, (opcode_t)BoolBinOp::And,

        LoadConst, ofalse,
        LoadConst, otrue,
        BinBool, (opcode_t)BoolBinOp::And,

        LoadConst, otrue,
        LoadConst, otrue,
        BinBool, (opcode_t)BoolBinOp::And,

        LoadConst, ofalse,
        LoadConst, otrue,
        BinBool, (opcode_t)BoolBinOp::Or,

        LoadConst, otrue,
        LoadConst, otrue,
        BinBool, (opcode_t)BoolBinOp::Or,

        LoadConst, ofalse,
        LoadConst, ofalse,
        BinBool, (opcode_t)BoolBinOp::Or,

        // return TOS
        // LoadConst, fnull,
        Return,
    };

    // --- Runtime ---
    interpret(frame);
    cout << "---" << endl;

    if (on_error()) {
        cerr << ">>> On error !!!" << endl;
        return;
    }

    cout << "* End frame : " << endl;
    print(frame);

    cout << "* End stack :" << endl;
    int stki = Program::instance->obj_stack.size();
    for (auto o : Program::instance->obj_stack) {
        cout << --stki << ". ";
        print(o);
    }
}
