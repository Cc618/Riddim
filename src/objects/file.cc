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

        CHECK_NOKWARGS("File");

        str_t open_mode = "r";
        if (args_data.size() == 2) {
            if (args_data[1]->type != Str::class_type) {
                THROW_TYPE_ERROR_PREF("File.@new{mode}", args_data[1]->type,
                                      Str::class_type);

                return nullptr;
            }

            open_mode = reinterpret_cast<Str *>(args_data[1])->data;
        }

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

        // Open mode
        if (open_mode == "r") {
            result->mode_read = true;
            result->mode_write = false;
            result->mode_binary = false;
        } else if (open_mode == "w") {
            result->mode_read = false;
            result->mode_write = true;
            result->mode_binary = false;
        } else if (open_mode == "rb") {
            result->mode_read = true;
            result->mode_write = false;
            result->mode_binary = true;
        } else if (open_mode == "wb") {
            result->mode_read = false;
            result->mode_write = true;
            result->mode_binary = true;
        } else if (open_mode == "rw") {
            result->mode_read = true;
            result->mode_write = true;
            result->mode_binary = false;
        } else if (open_mode == "rwb") {
            result->mode_read = true;
            result->mode_write = true;
            result->mode_binary = true;
        } else {
            throw_fmt(FileError, "Invalid open mode %s%s%s", C_RED, open_mode,
                      C_NORMAL);

            return nullptr;
        }

        ios_base::openmode mode = static_cast<ios_base::openmode>(0);

        if (result->mode_read && result->mode_write) {
            mode |= fstream::in;
            mode |= fstream::out;
        } else if (result->mode_write) {
            mode |= fstream::out;
            mode |= fstream::trunc;
        } else if (result->mode_read) {
            mode |= fstream::in;
        }

        if (result->mode_binary) {
            mode |= fstream::binary;
        }

        // Open file
        result->data.open(result->path, mode);

        if (!result->data.is_open()) {
            throw_fmt(FileError, "File %s%s%s failed to opened", C_BLUE,
                      result->path.c_str(), C_NORMAL);

            return nullptr;
        }

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
        "Writes the textual / binary content to the file\n\n- s, Str (text "
        "mode) or Iterable of Int (binary mode) : Content";
    method_write->doc_signature = {{"s", false}};
}

// --- Methods ---
Object *File::me_close_handler(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(File, "close");

    CHECK_NOARGS("File.close");
    CHECK_NOKWARGS("File.close");

    me->data.close();

    return null;
}

Object *File::me_read_handler(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(File, "read");

    CHECK_NOARGS("File.read");
    CHECK_NOKWARGS("File.read");

    // String content
    if (me->data.is_open()) {
        if (me->mode_binary) {
            // TODO A : Binary mode
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

    throw_fmt(FileError, "File %s%s%s not opened", C_BLUE, me->path.c_str(),
              C_NORMAL);

    return nullptr;
}

Object *File::me_write_handler(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(File, "write");

    CHECK_ARGSLEN(1, "File.write");
    CHECK_NOKWARGS("File.write");

    if (me->mode_binary) {
        // TODO A :
    } else {
        if (args_data[0]->type != Str::class_type) {
            THROW_TYPE_ERROR_PREF("File.write", args_data[0]->type,
                                  Str::class_type);

            return nullptr;
        }

        auto data = reinterpret_cast<Str*>(args_data[0])->data;

        me->data << data;

        return null;
    }

    return nullptr;
}
