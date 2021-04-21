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

// --- Decls ---
FnDecl::Args::~Args() {
    for (const auto &[k, v] : args) {
        if (v)
            delete v;
    }
}

FnDecl::~FnDecl() {
    delete body;
    delete target;
    delete args;
}

void FnDecl::debug(int indent) {
    cout << str_indent(indent) << "FnDecl(" << endl;
    target->debug(indent + 1);
    body->debug(indent + 1);
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

// --- Stmts ---
WhileStmt::WhileStmt(Exp *condition, Block *body)
    : LoopStmt(condition->fileline), condition(condition), body(body) {}

WhileStmt::~WhileStmt() {
    delete condition;
    delete body;
}

void WhileStmt::debug(int indent) {
    cout << str_indent(indent) << "WhileStmt(" << endl;
    cout << str_indent(indent + 1) << "line=" << fileline << endl;
    condition->debug(indent + 1);
    body->debug(indent + 1);
    cout << str_indent(indent) << ")" << endl;
}

IfStmt::IfStmt(Exp *condition, Block *ifbody)
    : Stmt(condition->fileline), condition(condition), ifbody(ifbody) {}

IfStmt::~IfStmt() {
    delete condition;
    delete ifbody;

    if (elsebody)
        delete elsebody;
}

void IfStmt::debug(int indent) {
    cout << str_indent(indent) << "IfStmt(" << endl;
    cout << str_indent(indent + 1) << "line=" << fileline << endl;
    condition->debug(indent + 1);
    ifbody->debug(indent + 1);
    if (elsebody)
        elsebody->debug(indent + 1);
    cout << str_indent(indent) << ")" << endl;
}

TryStmt::TryStmt(Block *trybody)
    : Stmt(trybody->fileline), trybody(trybody) {}

TryStmt::~TryStmt() {
    delete trybody;

    for (auto &clause : catchbodies) {
        if (clause.type) delete clause.type;
        delete clause.body;
    }
}

void TryStmt::debug(int indent) {
    cout << str_indent(indent) << "IfStmt(" << endl;
    cout << str_indent(indent + 1) << "line=" << fileline << endl;
    trybody->debug(indent + 1);
    cout << str_indent(indent) << ")" << endl;
}

ReturnStmt::ReturnStmt(line_t fileline, Exp *exp)
    : Stmt(fileline), exp(exp) {}

ReturnStmt::~ReturnStmt() {
    if (exp)
        delete exp;
}

void ReturnStmt::debug(int indent) {
    cout << str_indent(indent) << "ReturnStmt(" << endl;
    cout << str_indent(indent + 1) << "line=" << fileline << endl;
    if (exp)
        exp->debug(indent + 1);
    cout << str_indent(indent) << ")" << endl;
}

LoopControlStmt::LoopControlStmt(line_t fileline, bool isbreak)
    : Stmt(fileline), isbreak(isbreak) {}

void LoopControlStmt::debug(int indent) {
    cout << str_indent(indent) << "LoopControlStmt(" << endl;
    cout << str_indent(indent + 1) << "line=" << fileline << endl;
    cout << str_indent(indent) << ")" << endl;
}

void RethrowStmt::debug(int indent) {
    cout << str_indent(indent) << "RethrowStmt()" << endl;
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
Set::Set(line_t fileline, Target *target, Exp *exp)
    : Exp(fileline), target(target), exp(exp) {}

Set::~Set() {
    delete target;
    delete exp;
}

void Set::debug(int indent) {
    cout << str_indent(indent) << "Set(" << endl;
    target->debug(indent + 1);
    exp->debug(indent + 1);
    cout << str_indent(indent) << ")" << endl;
}

CallExp::CallExp(line_t fileline)
    : Exp(fileline) {}

CallExp::~CallExp() {
    delete exp;

    for (auto arg : args)
        delete arg;
}

void CallExp::debug(int indent) {
    cout << str_indent(indent) << "Call(" << endl;
    exp->debug(indent + 1);
    for (auto arg : args)
        arg->debug(indent + 1);
    cout << str_indent(indent) << ")" << endl;
}

ASTNode **CallExp::fetch_cascade_id() {
    auto id = dynamic_cast<Id*>(exp);

    if (id) {
        return reinterpret_cast<ASTNode**>(&exp);
    }

    return nullptr;
}

VecLiteral::~VecLiteral() {
    for (auto exp : exps)
        delete exp;
}

void VecLiteral::debug(int indent) {
    cout << str_indent(indent) << "VecLiteral(" << endl;
    for (auto exp : exps)
        exp->debug(indent + 1);
    cout << str_indent(indent) << ")" << endl;
}

MapLiteral::~MapLiteral() {
    for (const auto &[k, v] : kv) {
        delete k;
        delete v;
    }
}

void MapLiteral::debug(int indent) {
    cout << str_indent(indent) << "MapLiteral(" << endl;
    for (const auto &[k, v] : kv) {
        k->debug(indent + 1);
        v->debug(indent + 1);
    }
    cout << str_indent(indent) << ")" << endl;
}

Attr::~Attr() { delete exp; }

void Attr::debug(int indent) {
    cout << str_indent(indent) << "Attr(" << endl;
    cout << str_indent(indent + 1) << "attr=" << attr << endl;
    exp->debug(indent + 1);
    cout << str_indent(indent) << ")" << endl;
}

Indexing::~Indexing() {
    delete container;
    delete index;
}

void Indexing::debug(int indent) {
    cout << str_indent(indent) << "Indexing(" << endl;
    cout << str_indent(indent + 1) << "container=";
    container->debug(indent + 1);
    cout << str_indent(indent + 1) << "index=";
    index->debug(indent + 1);
    cout << str_indent(indent) << ")" << endl;
}

void Id::debug(int indent) {
    cout << str_indent(indent) << "Id(" << id << ")" << endl;
}

Const::Const(line_t fileline, Type type, const const_val_t &val)
    : Exp(fileline), type(type), val(val) {}

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

Cascade::Cascade(line_t fileline, Exp *left, Exp *right)
    : Exp(fileline), left(left), right(right) {}

Cascade::~Cascade() {
    delete left;
    delete right;
}

void Cascade::debug(int indent) {
    cout << str_indent(indent) << "Cascade(" << endl;
    left->debug(indent + 1);
    right->debug(indent + 1);
    cout << str_indent(indent) << ")" << endl;
}

ASTNode **Cascade::fetch_cascade_id() {
    return right->fetch_cascade_id();
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

RelativeSet::RelativeSet(line_t fileline, Target *target, BinExp::Op op, Exp *exp)
    : Exp(fileline), target(target), op(op), exp(exp) {}

RelativeSet::~RelativeSet() { delete target; delete exp; }

void RelativeSet::debug(int indent) {
    cout << str_indent(indent) << "RelativeSet<" << op << ">(" << endl;
    target->debug(indent + 1);
    exp->debug(indent + 1);
    cout << str_indent(indent) << ")" << endl;
}

// --- Targets ---
void IdTarget::debug(int indent) {
    cout << str_indent(indent) << "IdTarget(" << endl;
    id->debug(indent + 1);
    cout << str_indent(indent) << ")" << endl;
}

void IndexingTarget::debug(int indent) {
    cout << str_indent(indent) << "IndexingTarget(" << endl;
    indexing->debug(indent + 1);
    cout << str_indent(indent) << ")" << endl;
}

void AttrTarget::debug(int indent) {
    cout << str_indent(indent) << "AttrTarget(" << endl;
    attr->debug(indent + 1);
    cout << str_indent(indent) << ")" << endl;
}
} // namespace ast
