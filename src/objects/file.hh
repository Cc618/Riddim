#pragma once

// File handler

#include <fstream>
#include <iostream>
#include "object.hh"
#include "methods.hh"

struct File : public DynamicObject {
    static DynamicType *class_type;

    static Object *stdin;
    static Object *stdout;
    static Object *stderr;

    // Kind of file since cin / cout / cerr are not fstreams
    enum FileKind {
        Empty,
        // Default, normal file
        Data,
        Stdin,
        Stdout,
        Stderr,
    } kind = FileKind::Empty;

    std::fstream data;

    // Path or id
    str_t path;
    bool mode_read = false;
    bool mode_write = false;
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

protected:
    File();
};

