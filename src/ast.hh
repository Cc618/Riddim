#pragma once

#include "utils.hh"
#include <string>
#include <variant>
#include <vector>

struct Module;
#define ModuleObject Module

namespace ast {
struct ASTNode {
    // Line of the start of this node in the source file
    line_t fileline;

    ASTNode(line_t fileline = 0) : fileline(fileline) {}

    virtual ~ASTNode() = default;

    // Prints recursively all nodes
    virtual void debug(int indent = 0) = 0;

    // Generates the code of this node (and its children) for this module
    // * Note that every body of this function is declared in codegen.cc NOT
    // ast.cc
    virtual void gen_code(ModuleObject *module) = 0;
};

struct Block;
struct Stmt;
struct Set;
struct Exp;

struct AstModule : public ASTNode {
    // TODO
    // Can be nullptr if there is no body (empty file)
    Block *content = nullptr;
    str_t filename;

    AstModule(line_t fileline) : ASTNode(fileline) {}

    virtual ~AstModule();

    virtual void debug(int indent = 0) override;

    virtual void gen_code(ModuleObject *module) override;
};

// --- Stmts ---
struct Block : public ASTNode {
    std::vector<Stmt *> stmts;

    Block(line_t fileline) : ASTNode(fileline) {}

    virtual ~Block();

    virtual void debug(int indent = 0) override;

    virtual void gen_code(ModuleObject *module) override;
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

    virtual void gen_code(ModuleObject *module) override;
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

    virtual void gen_code(ModuleObject *module) override;
};

// Literal
struct Const : public Exp {
    enum Type {
        Int,
        Str,
    } type;

    std::variant<str_t, int_t> val;

    Const(line_t fileline, int_t val);
    Const(line_t fileline, const str_t &val);

    virtual void debug(int indent = 0) override;

    virtual void gen_code(ModuleObject *module) override;
};

// + * etc.
struct BinExp : public Exp {
    // TODO
    enum Op {
        Add,
        // Sub,
        Mul,
        // Div,
        // Mod,
        // And,
        // Or,
    };

    Op op;
    Exp *left;
    Exp *right;

    BinExp(line_t fileline, Exp *left, Op op, Exp *right);

    ~BinExp();

    virtual void debug(int indent = 0) override;

    virtual void gen_code(ModuleObject *module) override;
};
} // namespace ast
