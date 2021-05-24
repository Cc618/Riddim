#include "file.hh"
#include "bool.hh"
#include "builtins.hh"
#include "error.hh"
#include "float.hh"
#include "function.hh"
#include "hash.hh"
#include "int.hh"
#include "iterator.hh"
#include "map.hh"
#include "null.hh"
#include "program.hh"
#include "str.hh"
#include "vec.hh"
#include <cstring>

using namespace std;

DynamicType *File::class_type = nullptr;

Object *File::stdin = nullptr;
Object *File::stdout = nullptr;
Object *File::stderr = nullptr;

File *File::New() {
    auto self = new (nothrow) File();

    if (!self) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    DynamicObject::init(self);

    if (on_error())
        return nullptr;

    return self;
}

File::File() : DynamicObject(File::class_type) {}

File::~File() {
    if (data.is_open()) {
        data.close();
    }
}

void File::init_class_type() {
    class_type = DynamicType::New("File");

    if (!class_type) {
        return;
    }

    class_type->constructor = [](Object *self, Object *args,
                                 Object *kwargs) -> Object * {
        INIT_METHOD(File, "File");

        // TODO A : Open mode
        CHECK_ARGSLEN(1, "File");
        CHECK_NOKWARGS("File");

        if (args_data[0]->type != Str::class_type) {
            THROW_TYPE_ERROR_PREF("File.@new{path}", args_data[0]->type,
                                  Str::class_type);

            return nullptr;
        }

        auto result = File::New();

        // Dispatch error
        if (!result)
            return nullptr;

        result->path = reinterpret_cast<Str *>(args_data[0])->data;

        result->mode_read = true;
        result->mode_write = false;
        result->mode_binary = false;

        // Open file
        result->data.open(result->path,
                          static_cast<ios_base::openmode>(
                              (result->mode_read ? fstream::in : 0) |
                              (result->mode_write ? fstream::out | fstream::trunc : 0) |
                              (result->mode_binary ? fstream::binary : 0)));
        // TODO A : File errors
        auto a = result->data.is_open();

        return result;
    };

    // @str
    class_type->fn_str = [](Object *self) -> Object * {
        auto me = reinterpret_cast<File *>(self);
        str_t repr = "File(" + me->path + ")";

        auto s = Str::New(repr);
        if (!s) {
            return nullptr;
        }

        return s;
    };
}

void File::init_class_objects() {
    // TODO A : stdin, stderr, stdout + global

    // Init methods
    NEW_METHOD(File, close);
    method_close->doc_str = "Closes the file";
    method_close->doc_signature = {};

    NEW_METHOD(File, read);
    method_read->doc_str = "Reads the content of the file";
    method_read->doc_signature = {};

    NEW_METHOD(File, write);
    method_write->doc_str =
        "Writes the textual content to the file\n\n- s, Str : Content";
    method_write->doc_signature = {{"s", false}};

    NEW_METHOD(File, write_binary);
    method_write_binary->doc_str = "Writes the binary content to the file\n\n- "
                                   "b, Iterable of Int : Binary buffer";
    method_write_binary->doc_signature = {{"s", false}};
}

// --- Methods ---
Object *File::me_close_handler(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(File, "close");

    CHECK_NOARGS("File.close");
    CHECK_NOKWARGS("File.close");

    // TODO A :
    me->data.close();

    return null;
}

Object *File::me_read_handler(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(File, "read");

    CHECK_NOARGS("File.read");
    CHECK_NOKWARGS("File.read");

    // TODO A : me->mode_open

    // String content
    if (me->data.is_open()) {
        if (me->mode_binary) {
            // TODO : Binary mode
        } else {
            str_t content;
            str_t sline;
            while (getline(me->data, sline)) {
                content += sline;
            }

            auto result = Str::New(content);

            if (!result) {
                return nullptr;
            }

            return result;
        }
    }
    // TODO : Error otherwise

    return nullptr;
}

Object *File::me_write_handler(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(File, "write");

    CHECK_ARGSLEN(1, "File.write");
    CHECK_NOKWARGS("File.write");

    if (args_data[0]->type != Str::class_type) {
        THROW_TYPE_ERROR_PREF("File.write", args_data[0]->type,
                              Str::class_type);

        return nullptr;
    }

    // TODO A :

    return null;
}

Object *File::me_write_binary_handler(Object *self, Object *args,
                                      Object *kwargs) {
    INIT_METHOD(File, "write_binary");

    CHECK_ARGSLEN(1, "File.write_binary");
    CHECK_NOKWARGS("File.write_binary");

    // TODO A :

    return null;
}