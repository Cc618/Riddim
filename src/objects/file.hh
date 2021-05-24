#pragma once

// File handler

#include <fstream>
#include "object.hh"
#include "methods.hh"

struct File : public DynamicObject {
    static DynamicType *class_type;

    static Object *stdin;
    static Object *stdout;
    static Object *stderr;

    std::fstream data;

    // Path or id
    str_t path;
    bool mode_read = true;
    bool mode_write = true;
    bool mode_binary = false;

    virtual ~File();

    static File *New();

    // Can throw
    static void init_class_type();

    static void init_class_objects();

    // Methods
    DECL_METHOD(close);
    DECL_METHOD(read);
    DECL_METHOD(write);
    DECL_METHOD(write_binary);

protected:
    File();
};

