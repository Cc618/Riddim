#pragma once

#include <string>
#include <variant>
#include <vector>
#include "utils.hh"

struct ASTNode {
    // Line of the start of this node in the source file
    line_t fileline;

    ASTNode(line_t fileline=0) : fileline(fileline) {}

    virtual ~ASTNode() = default;

    // Prints recursively all nodes
    virtual void debug(int indent=0) = 0;
};

struct Block;
struct Stmt;
struct Set;
struct Exp;

// TODO : Location
// TODO : Memory leaks
struct Module : public ASTNode {
    Block *content;
    str_t filename;

    Module(line_t fileline) : ASTNode(fileline) {}

    virtual ~Module();

    virtual void debug(int indent=0) override;
};

struct Block : public ASTNode {
    std::vector<Stmt *> stmts;

    Block(line_t fileline) : ASTNode(fileline) {}

    virtual ~Block();

    virtual void debug(int indent=0) override;
};

struct Stmt : public ASTNode {
    using ASTNode::ASTNode;
};

// TODO
struct Set : public Stmt {
    str_t id;
    Exp *exp;

    Set(line_t fileline, const str_t &id, Exp *exp) : Stmt(fileline), id(id), exp(exp) {}

    virtual ~Set();

    virtual void debug(int indent=0) override;
};

struct Exp : public ASTNode {
    using ASTNode::ASTNode;
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

    virtual void debug(int indent=0) override;
};
