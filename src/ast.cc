#include "ast.hh"
#include "utils.hh"
#include <iostream>

using namespace std;

// --- Utils ---
// Returns the indentation (for debug)
inline string str_indent(int indent) { return string(indent * 2, ' '); }

// Default debug for a leaf node
#define LEAF_DEBUG(cls)                                                        \
    void cls::debug(int indent) {                                              \
        cout << str_indent(indent) << #cls << "()" << endl;                    \
    }

// --- Utils ---
Module::~Module() { delete content; }

void Module::debug(int indent) {
    cout << str_indent(indent) << "Module(" << endl;
    content->debug(indent + 1);
    cout << str_indent(indent) << ")" << endl;
}

// --- Stmts ---
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

// --- Exps ---
Set::~Set() { delete exp; }

void Set::debug(int indent) {
    cout << str_indent(indent) << "Set(" << endl;
    cout << str_indent(indent + 1) << "line=" << fileline << endl;
    cout << str_indent(indent + 1) << "id=" << id << endl;
    exp->debug(indent + 1);
    cout << str_indent(indent) << ")" << endl;
}

Const::Const(line_t fileline, long long val)
    : Exp(fileline), type(Int), val(val) {}

Const::Const(line_t fileline, const str_t &val)
    : Exp(fileline), type(Str), val(val) {}

void Const::debug(int indent) {
    str_t strval = "???";
    switch (type) {
    case Int:
        strval = to_string(get<long long>(val));
        break;

    case Str:
        strval = string_repr(get<str_t>(val));
        break;
    }
    cout << str_indent(indent) << "Set(" << strval << ")" << endl;
}

BinExp::BinExp(line_t fileline, Exp *left, char op, Exp *right)
    : Exp(fileline), left(left), op(op), right(right) {}

BinExp::~BinExp() {
    delete left;
    delete right;
}

void BinExp::debug(int indent) {
    cout << str_indent(indent) << op << "(" << endl;
    left->debug(indent + 1);
    right->debug(indent + 1);
    cout << str_indent(indent) << ")" << endl;
}