#include "ast.hh"
#include "utils.hh"
#include <iostream>

using namespace std;

namespace ast {
// --- Utils ---
// Returns the indentation (for debug)
inline string str_indent(int indent) { return string(indent * 2, ' '); }

// Default debug for a leaf node
#define LEAF_DEBUG(cls)                                                        \
    void cls::debug(int indent) {                                              \
        cout << str_indent(indent) << #cls << "()" << endl;                    \
    }

// --- Utils ---
AstModule::~AstModule() { delete content; }

void AstModule::debug(int indent) {
    cout << str_indent(indent) << "AstModule(" << endl;
    if (content)
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

ExpStmt::ExpStmt(Exp *exp) : Stmt(exp->fileline), exp(exp) {}

ExpStmt::~ExpStmt() { delete exp; }

void ExpStmt::debug(int indent) {
    cout << str_indent(indent) << "ExpStmt(" << endl;
    cout << str_indent(indent + 1) << "line=" << fileline << endl;
    exp->debug(indent + 1);
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

void Id::debug(int indent) {
    cout << str_indent(indent) << "Id(" << id << ")" << endl;
}

Const::Const(line_t fileline, Type type) : Exp(fileline), type(type) {}

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

BinExp::BinExp(line_t fileline, Exp *left, Op op, Exp *right)
    : Exp(fileline), left(left), op(op), right(right) {}

BinExp::~BinExp() {
    delete left;
    delete right;
}

void BinExp::debug(int indent) {
    cout << str_indent(indent) << "BinExp<" << op << ">(" << endl;
    left->debug(indent + 1);
    right->debug(indent + 1);
    cout << str_indent(indent) << ")" << endl;
}

UnaExp::UnaExp(line_t fileline, Exp *exp, Op op)
    : Exp(fileline), exp(exp), op(op) {}

UnaExp::~UnaExp() { delete exp; }

void UnaExp::debug(int indent) {
    cout << str_indent(indent) << "UnaExp<" << op << ">(" << endl;
    exp->debug(indent + 1);
    cout << str_indent(indent) << ")" << endl;
}
} // namespace ast
