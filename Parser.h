#ifndef PROVER_H_INCLUDED
#define PROVER_H_INCLUDED

#include<vector>
#include<unordered_map>
#include<unordered_set>
#include<string>

using namespace std;

typedef int Predicate;

// 
struct Mapping{
    vector<int> clauseind;
    vector<int> clausepos;
    int size;
};

struct Argument{
    bool isvariable;
    string id;
};

struct Literal{
    bool istrue;
    string predicate;
    vector<Argument> arguments;
};

struct Clause{
    vector<Literal> literals;
    unordered_set<int> composition;
};

struct KB{
    // int size;
    vector<Clause> clauses;
    unordered_map<string,Mapping> index;
};

enum Op{
	OP_AND,
	OP_OR,
	OP_NOT,
	OP_IMPLY,
	OP_DEF
};

struct TreeNode{
    Op op;
    TreeNode* left;
    int l,r;
    TreeNode* right;
    string context;
    TreeNode(): op(OP_DEF), left(NULL), right(NULL), context(""), l(0), r(0) {}
};

class FolParser{
public:
    FolParser(vector<string> folkb, vector<string> folq);
    ~FolParser();
    
    KB getKB();
    std::vector<Clause> getQuery();

protected:
    KB kb;
    std::vector<Clause> queries;
    
    bool parse(string &s, TreeNode* tn);
    bool convert(TreeNode*& tn);
    void negate(TreeNode*& tn);
    void impElim(TreeNode* tn);
    void notInwd(TreeNode*& tn);
    void andDstb(TreeNode* tn);
    void archiveKB(TreeNode* tn, Clause* c);
    void archiveQ(TreeNode* tn, Clause* c);
    Literal parseLiteral(TreeNode* tn);
};


#endif