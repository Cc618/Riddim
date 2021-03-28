#include "ast.hh"
#include <iostream>

using namespace std;

// Returns the indentation (for debug)
inline string str_indent(int indent) { return string(indent * 2, ' '); }

// Default debug for a leaf node
#define LEAF_DEBUG(cls)                                                        \
    void cls::debug(int indent) {                                              \
        cout << str_indent(indent) << #cls << "()" << endl;                    \
    }

Module::~Module() { delete content; }

void Module::debug(int indent) {
    cout << str_indent(indent) << "Module(" << endl;
    content->debug(indent + 1);
    cout << str_indent(indent) << ")" << endl;
}

Block::~Block() {
    for (auto stmt : stmts)
        delete stmt;
}

void Block::debug(int indent) {
    cout << str_indent(indent) << "Block(" << endl;

    for (auto stmt : stmts)
        stmt->debug(indent + 1);

    cout << str_indent(indent) << ")" << endl;
}

Set::~Set() { delete exp; }

void Set::debug(int indent) {
    cout << str_indent(indent) << "Set(" << endl;
    cout << str_indent(indent + 1) << "id=" << id << endl;
    exp->debug(indent + 1);
    cout << str_indent(indent) << ")" << endl;
}

Const::Const(long long val) : type(Int), val(val) {}

Const::Const(const std::string &val) : type(Str), val(val) {}

void Const::debug(int indent) {
    string strval = "???";
    switch (type)
    {
    case Int:
        strval = to_string(get<long long>(val));
        break;

    case Str:
        strval = "'" + get<string>(val) + "'";
        break;
    }
    cout << str_indent(indent) << "Set(" << strval << ")" << endl;
}