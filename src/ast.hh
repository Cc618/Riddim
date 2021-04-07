#pragma once

#include "utils.hh"
#include <string>
#include <variant>
#include <vector>

namespace ast {
struct ASTNode {
    // Line of the start of this node in the source file
    line_t fileline;

    ASTNode(line_t fileline = 0) : fileline(fileline) {}

    virtual ~ASTNode() = default;

    // Prints recursively all nodes
    virtual void debug(int indent = 0) = 0;
};

struct Block;
struct Stmt;
struct Set;
struct Exp;

struct Module : public ASTNode {
    // TODO
    // Can be nullptr if there is no body (empty file)
    Block *content = nullptr;
    str_t filename;

    Module(line_t fileline) : ASTNode(fileline) {}

    virtual ~Module();

    virtual void debug(int indent = 0) override;
};

// --- Stmts ---
struct Block : public ASTNode {
    std::vector<Stmt *> stmts;

    Block(line_t fileline) : ASTNode(fileline) {}

    virtual ~Block();

    virtual void debug(int indent = 0) override;
};

struct Stmt : public ASTNode {
    using ASTNode::ASTNode;
};

// A statement made of an expression
struct ExpStmt : public Stmt {
    Exp *exp;

    ExpStmt(Exp *exp);

    virtual ~ExpStmt();

    virtual void debug(int indent = 0) override;
};

// --- Exps ---
struct Exp : public ASTNode {
    using ASTNode::ASTNode;
};

// Assignment
struct Set : public Exp {
    str_t id;
    Exp *exp;

    Set(line_t fileline, const str_t &id, Exp *exp)
        : Exp(fileline), id(id), exp(exp) {}

    virtual ~Set();

    virtual void debug(int indent = 0) override;
};

// Literal
struct Const : public Exp {
    enum Type {
        Int,
        Str,
    } type;

    std::variant<str_t, long long> val;

    Const(line_t fileline, long long val);
    Const(line_t fileline, const str_t &val);

    virtual void debug(int indent = 0) override;
};

// + * etc.
struct BinExp : public Exp {
    // '+' '*' etc.
    char op;
    Exp *left;
    Exp *right;

    BinExp(line_t fileline, Exp *left, char op, Exp *right);

    ~BinExp();

    virtual void debug(int indent = 0) override;
};
} // namespace ast
