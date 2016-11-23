#include"Parser.h"
#include<iostream>
#include <fstream>

using namespace std;


bool unifiable(Literal l1, Literal l2, unordered_map<int, Argument> &sub){
    
    return false;
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
    int ind = kb.clauses.size();
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
        
        Mapping mq = kb.index[ql.predicate];
        for(int i  = 0; i< mq.size;++i){
            if(qc.composition.find(mq.clauseind[i]) != qc.composition.end()) continue;   // this clause is already used during the construction of qc.
            Clause ci(kb.clauses[mq.clauseind[i]]);
            // check if unifiable
            unordered_map<int, Argument> sub;
            if(unifiable(ql, ci.literals[mq.clausepos[i]],sub)){
                Clause cq(qc);
                deduct(cq, i);
                deduct(ci, mq.clausepos[i]);           // deduct the opposite literal from the clauses.
                if(concatenate(cq, ci)) return true;    // add ci into cq.    if nothing left, the query succeeds.
                toquery.push_back(kb.clauses.size());           // add to a queue, later query them.
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
    ifstream t("../test.txt");
    std::vector<string> folkb, folq;
    
    /*
        save the lines of kb and query
        */
    int lines_kb, lines_q;
    string s_lines_kb, s_lines_q;
    
    if(!getline(t, s_lines_q)) return 1;
    lines_q =stoi(s_lines_q);
    
    folq.reserve(lines_q);
    for(int i = 0; i<lines_q; ++i){
        string temp_s;
        getline(t, temp_s);
        folq.push_back(temp_s);
    }
    
    getline(t, s_lines_kb);
    lines_kb =stoi(s_lines_kb);
    
    folkb.reserve(lines_kb);
    for(int i = 0; i<lines_kb; ++i){
        string temp_s;
        getline(t, temp_s);
        folkb.push_back(temp_s);
    }

    // FolParser *parser = new FolParser();
    
    return 0;
}