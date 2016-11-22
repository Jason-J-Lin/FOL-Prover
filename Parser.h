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
    Predicate predicate;
    string name;
    vector<Argument> arguments;
};

struct Clause{
    vector<Literal> literals;
    unordered_set<int> composition;
};

struct KB{
    // int size;
    vector<Clause> clauses;
    unordered_map<Predicate,Mapping> index;
};

class FolParser{
public:
    FolParser(string filename);
    
    KB getKB();
    std::vector<Clause> getQuery();

private:
    KB kb;
    std::vector<Clause> queries;
};


#endif