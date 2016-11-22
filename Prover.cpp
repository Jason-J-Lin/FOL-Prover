#include<iostream>
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

bool unifiable(Literal l1, Literal l2, unordered_map<int, Argument> &sub){
    
}

void unify(Clause c1, Clause c2, unordered_map<int, Argument> &sub){
    
}

void deduct(Clause &c, int pos){
    c.literals.erase(c.literals.begin()+pos);
}

/*  concatenate two clauses,
    1. add all the literals.
    2. add the composition record from both clauses.
    3. return true if nothing left.
    */
bool concatenate(Clause &c1, Clause &c2){
    if(c1.literals.empty()&&c2.literals.empty()) return true;
    c1.literals.insert(c1.literals.end(), c2.literals.begin(), c2.literals.end());
    c1.composition.insert(c2.composition.begin(), c2.composition.end());
    return false;
}

/*  add the clause back to kb,
    1. add clause to kb.
    2. update the indexing map of kb.
    */
void tellKB(KB &kb, const Clause &c){
    int ind = kb.size();
    kb.clauses.push_back(c);
    for(int i = 0; i < c.literals.size(); ++i){
        if(kb.index.find(c.literals[i].predicate)!=kb.index.end()){
            kb.index[c.literals[i].predicate].size++;
            kb.index[c.literals[i].predicate].clauseind.push_back(ind);
            kb.index[c.literals[i].predicate].clausepos.push_back(i);
        }
    }
}

/*  looks like a backward chaining pattern*/
bool query(KB &kb, Clause qc){
    vector<int> toquery;
    for(Literal ql : qc.literals){
        if(kb.index.find(ql.predicate) == kb.index.end()) continue;
        if(qc.composition.find(kb.index[ql.predicate]) != qc.composition.end()) continue;   // this clause is already used during the construction of qc.
        Mapping mq = kb.index[ql.predicate];
        for(int i  = 0; i< mq.size();++i){
            Clause ci(kb.clauses[mq.clauseind[i]]);
            // check if unifiable
            unordered_map<int, Argument> sub;
            if(unifiable(ql, ci[mq.clausepos[i]],sub)){
                Clause cq(qc);
                deduct(cq, i);
                deduct(ci, mq.clausepos[i]));           // deduct the opposite literal from the clauses.
                if(concatenate(cq, ci)) return true;    // add ci into cq.    if nothing left, the query succeeds.
                toquery.push_back(kb.size());           // add to a queue, later query them.
                tellKB(kb, cq);                         // put new clause back to kb.
            }
        }
    }
    for(int i : toquery){
        if(query(kb, kb.clauses[i])) return true;
    }
    return false;
}

int main(){
    Argument a1 = {false, "John"}, a2 = {true, "x"}, a3 = {true, "y"}, a4 = {false, "Bob"}, a5 = {true, "y"}, a6(a5);
    
    
    
    /*std::vector<Argument> p1 = {a1,a2,a3};
    Literal l1;
    l1.istrue = true;
    l1.predicate = 1;
    l1.arguments = p1;
    std::vector<Argument> p2 = {a2,a3,a4};
    Literal l2;
    l2.istrue = true;
    l2.predicate = 2;
    l2.arguments = p2;
    std::vector<Argument> p3 = {a1,a5};
    Literal l3;
    l3.istrue = true;
    l3.predicate = 1;
    l3.arguments = p3;
    
    Clause c1, c2;
    c1.literals.push_back(l1);
    c1.literals.push_back(l2);
    c2.literals.push_back(l1);
    
    KB kb;
    for(auto c : c1.literals) kb.index[c.predicate].push_back(kb.clauses.size());
    kb.clauses.push_back(c1);*/
    
    return 0;
}