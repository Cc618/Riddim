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

    // Don't forget to call this for inherited objects (at the top of the
    // function)
    // This updates line deltas in frame to output line information
    // during error
    virtual void gen_code(ModuleObject *module) override;
};

// While statement
struct WhileStmt : public Stmt {
    Exp *condition;
    Block *body;

    WhileStmt(Exp *condition, Block *body);

    virtual ~WhileStmt();

    virtual void debug(int indent = 0) override;

    virtual void gen_code(ModuleObject *module) override;
};

// If statement (with else etc.)
struct IfStmt : public Stmt {
    Exp *condition;
    Block *ifbody;
    Block *elsebody = nullptr;

    IfStmt(Exp *condition, Block *ifbody);

    virtual ~IfStmt();

    virtual void debug(int indent = 0) override;

    virtual void gen_code(ModuleObject *module) override;
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

// TODO : Not id, can be indexing...
// Assignment
struct Target;
struct Set : public Exp {
    Target *target;
    Exp *exp;

    Set(line_t fileline, Target *target, Exp *exp);

    virtual ~Set();

    virtual void debug(int indent = 0) override;

    virtual void gen_code(ModuleObject *module) override;
};

// Prints its expression
struct PrintExp : public Exp {
    std::vector<Exp *> exps;

    PrintExp(line_t fileline, const std::vector<Exp *> &exps);

    virtual ~PrintExp();

    virtual void debug(int indent = 0) override;

    virtual void gen_code(ModuleObject *module) override;
};

struct VecLiteral : public Exp {
    std::vector<Exp *> exps;

    VecLiteral(line_t fileline, const std::vector<Exp *> &exps)
        : Exp(fileline), exps(exps) {}

    virtual ~VecLiteral();

    virtual void debug(int indent = 0) override;

    virtual void gen_code(ModuleObject *module) override;
};

// Attribute reference (exp.id)
struct Attr : public Exp {
    Exp *exp;
    str_t attr;

    Attr(line_t fileline, Exp *exp, const str_t &attr)
        : Exp(fileline), exp(exp), attr(attr) {}

    virtual ~Attr();

    virtual void debug(int indent = 0) override;

    virtual void gen_code(ModuleObject *module) override;
};

// Get item (a[42])
struct Indexing : public Exp {
    Exp *container;
    Exp *index;

    Indexing(line_t fileline, Exp *container, Exp *index)
        : Exp(fileline), container(container), index(index) {}

    virtual ~Indexing();

    virtual void debug(int indent = 0) override;

    virtual void gen_code(ModuleObject *module) override;
};

// Variable
struct Id : public Exp {
    str_t id;

    Id(line_t fileline, const str_t &id) : Exp(fileline), id(id) {}

    virtual ~Id() = default;

    virtual void debug(int indent = 0) override;

    virtual void gen_code(ModuleObject *module) override;
};

struct Const;
typedef std::variant<str_t, int_t> const_val_t;

// Atom literal
struct Const : public Exp {
    enum Type {
        Int,
        Str,
        True,
        False,
        Null,
    } type;

    const_val_t val;

    // val is not set for the first constructor
    Const(line_t fileline, Type type, const const_val_t &val = const_val_t());
    Const(line_t fileline, int_t val);
    Const(line_t fileline, const str_t &val);

    virtual void debug(int indent = 0) override;

    virtual void gen_code(ModuleObject *module) override;
};

// Binary expression
// + * etc.
struct BinExp : public Exp {
    // TODO
    enum Op {
        Or,
        And,
        Equal,
        Lesser,
        Greater,
        LesserEqual,
        GreaterEqual,
        // Sub,
        Add,
        // Div,
        // Mod,
        Mul,
    };

    Op op;
    Exp *left;
    Exp *right;

    BinExp(line_t fileline, Exp *left, Op op, Exp *right);

    ~BinExp();

    virtual void debug(int indent = 0) override;

    virtual void gen_code(ModuleObject *module) override;
};

// Unary expression
// not etc.
struct UnaExp : public Exp {
    enum Op { Not };

    Op op;
    Exp *exp;

    UnaExp(line_t fileline, Exp *exp, Op op);

    ~UnaExp();

    virtual void debug(int indent = 0) override;

    virtual void gen_code(ModuleObject *module) override;
};

// --- Targets ---
// A target is an lvalue, it can be set in an assignment (Set)
// In gen_code, the value of the TOS is stored in this target but
// remains the TOS
struct Target : public ASTNode {
    Target(line_t fileline) : ASTNode(fileline) {}
};

// Target using an identifier
// a = ...
struct IdTarget : public Target {
    str_t id;

    IdTarget(line_t fileline, const str_t &id) : Target(fileline), id(id) {}

    virtual void debug(int indent = 0) override;

    virtual void gen_code(ModuleObject *module) override;
};

// Target using an index assignment
// container[index] = ...
struct IndexingTarget : public Target {
    Indexing *indexing;

    IndexingTarget(Indexing *indexing)
        : Target(indexing->fileline), indexing(indexing) {}

    virtual void debug(int indent = 0) override;

    virtual void gen_code(ModuleObject *module) override;
};

// Target using an attribute assignment
// exp.attr = ...
struct AttrTarget : public Target {
    Attr *attr;

    AttrTarget(Attr *attr)
        : Target(attr->fileline), attr(attr) {}

    virtual void debug(int indent = 0) override;

    virtual void gen_code(ModuleObject *module) override;
};
} // namespace ast
