#pragma once

#include <string>
#include <variant>
#include <vector>

struct ASTNode {
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

    virtual ~Module();
    virtual void debug(int indent=0) override;
};

struct Block : public ASTNode {
    std::vector<Stmt *> stmts;

    virtual ~Block();
    virtual void debug(int indent=0) override;
};

struct Stmt : public ASTNode {};

// TODO
struct Set : public Stmt {
    std::string id;
    Exp *exp;

    Set(const std::string &id, Exp *exp) : id(id), exp(exp) {}
    virtual ~Set();
    virtual void debug(int indent=0) override;
};

struct Exp : public ASTNode {};

// Literal
struct Const : public Exp {
    enum Type {
        Int,
        Str,
    } type;

    std::variant<std::string, long long> val;

    Const(long long val);
    Const(const std::string &val);
    virtual void debug(int indent=0) override;
};
