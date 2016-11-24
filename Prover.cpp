#include"Parser.h"
#include<iostream>
#include <fstream>

using namespace std;

#define DEBUG_PROVE

bool unifiable(Literal l1, Literal l2, unordered_map<string, Argument> &sub){
    if(l1.arguments.size()!= l2.arguments.size()){
        #ifdef DEBUG_PROVE
            cout<<l1.predicate<<"ERROR! argument count inconsistency"<<endl;    //  impossible to have same predicate name with different argument number.
        #endif
    }
    for(int i = 0; i< l1.arguments.size(); ++i){
        if(sub.find(l1.arguments[i].id)!=sub.end()) l1.arguments[i] = sub[l1.arguments[i].id];
        if(sub.find(l2.arguments[i].id)!=sub.end()) l2.arguments[i] = sub[l2.arguments[i].id];
        if(l1.arguments[i].isvariable && l2.arguments[i].isvariable){
            #ifdef DEBUG_UNIFY
                cout<<"unifying vars: "<<l1.arguments[i].id<<" "<<l2.arguments[i].id<<endl;    //  impossible to have same predicate name with different argument number.
            #endif
            if(l2.arguments[i].id == l1.arguments[i].id) continue;
            l2.arguments[i].id = l1.arguments[i].id;
            sub.insert({l2.arguments[i].id, l1.arguments[i]});
        }else if(l1.arguments[i].isvariable){
            #ifdef DEBUG_UNIFY
                cout<<"unifying var and const: "<<l1.arguments[i].id<<" "<<l2.arguments[i].id<<endl;    //  impossible to have same predicate name with different argument number.
            #endif
            sub.insert({l1.arguments[i].id, l2.arguments[i]});
        }else if(l2.arguments[i].isvariable){
            #ifdef DEBUG_UNIFY
                cout<<"unifying const and var: "<<l1.arguments[i].id<<" "<<l2.arguments[i].id<<endl;    //  impossible to have same predicate name with different argument number.
            #endif
            sub.insert({l2.arguments[i].id, l1.arguments[i]});
        }else if(l1.arguments[i].id != l2.arguments[i].id){
            return false;       // Constant mismatch, fail to unify
        }
    }
    // there may be many redundant links in 'sub', needs to resolute that
    return true;
}

// add multiple elimination here, try it!
void unify(Clause &c1, Clause &c2, unordered_map<string, Argument> &sub){
    for(auto l = c1.literals.begin(); l!= c1.literals.end(); ++l){
        for(auto a = l->arguments.begin(); a!= l->arguments.end(); ++a){
            if(sub.find(a->id)!= sub.end()) *a = sub[a->id];
        }
    }
    for(auto l = c2.literals.begin(); l!= c2.literals.end(); ++l){
        for(auto a = l->arguments.begin(); a!= l->arguments.end(); ++a){
            if(sub.find(a->id)!= sub.end()) *a = sub[a->id];
        }
    }
}

void deduct(Clause &c, int pos){
    if(pos >= c.literals.size()){
        #ifdef DEBUG_PROVE
            cout<<"ERROR! deduction index exceeds"<<endl;    //  should be impossible.
        #endif
    }
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

void renameVariable(KB &kb, int index){
    unordered_map<string,string> save;
    for(auto l = kb.clauses[index].literals.begin(); l != kb.clauses[index].literals.end(); ++l){
        for(auto a = l->arguments.begin(); a!= l->arguments.end(); ++a){
            if(!a->isvariable) continue;
            if(save.find(a->id) != save.end()) a->id = save[a->id];
            else{
                string newName = to_string(index)+"."+to_string(save.size());
                save.insert({a->id, newName});
                a->id = newName;
            }
        }
    }
}

/*  looks like a backward chaining pattern*/
bool query(KB &kb, Clause qc){
    vector<int> toquery;
    int ind_qc = 0;
    for(Literal ql : qc.literals){
        if(kb.index.find(ql.predicate) == kb.index.end()) continue;             // cannot find same predicate, abort
        
        Mapping mq = kb.index[ql.predicate];
        for(int i  = 0; i< mq.size;++i){
            if(qc.composition.find(mq.clauseind[i]) != qc.composition.end()){
                continue;       // this clause is already used during the construction of qc.
            }
            Clause ci(kb.clauses[mq.clauseind[i]]);
            if(ql.istrue == ci.literals[mq.clausepos[i]].istrue){
                // #ifdef DEBUG_PROVE
                // cout<<"ql: "<<ql.predicate<<" ci."<<i<<": "<<ci.literals[mq.clausepos[i]].predicate<<endl;
                // #endif
                continue;
            }
            // check if unifiable
            unordered_map<string, Argument> sub;
            // #ifdef DEBUG_PROVE
            //     cout<<"check unifiable: "<<l1.arguments[i].id<<" "<<l2.arguments[i].id<<endl;    //  impossible to have same predicate name with different argument number.
            // #endif
            if(unifiable(ql, ci.literals[mq.clausepos[i]],sub)){
                Clause cq(qc);
                cq.composition.insert(mq.clauseind[i]);
                unify(cq, ci, sub);
                deduct(cq, ind_qc);
                deduct(ci, mq.clausepos[i]);           // deduct the opposite literal from the clauses.
                if(concatenate(cq, ci)){
                    #ifdef DEBUG_PROVE
                        cout<<"****** proved! ******"<<endl;    //  impossible to have same predicate name with different argument number.
                    #endif
                    return true;    // add ci into cq.    if nothing left, the query succeeds.
                }
                toquery.push_back(kb.clauses.size());           // add to a queue, later query them.
                #ifdef DEBUG_PROVE
                    cout<<"adding: "<<i<<endl;    //  impossible to have same predicate name with different argument number.
                #endif
                tellKB(kb, cq);                         // put new clause back to kb.
                renameVariable(kb, kb.clauses.size()-1);
            }
        }
        ++ind_qc;
    }
    for(int i : toquery){
        #ifdef DEBUG_PROVE
            cout<<"querying: "<<i<<endl;    //  impossible to have same predicate name with different argument number.
        #endif
        if(query(kb, kb.clauses[i])) return true;
    }
    return false;
}

int main(){
    /*
        read file
        */
    ifstream t("./test.txt");
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
    
    /*
        let it be parsed into defined cnf structs
        */
    FolParser *parser = new FolParser(folkb,folq);
    KB kb = parser->getKB();
    vector<Clause> queries = parser->getQuery();
    
    for(int i = 0; i < kb.clauses.size(); ++i){
        renameVariable(kb, i);
    }
    /*
        write to output files
        */
    ofstream output("output.txt");
    for(Clause c : queries){
        KB temp_kb(kb);
        if(query(temp_kb, c)) output<<"TRUE"<<endl;
        else output<<"FALSE"<<endl;
    }
    output.close();
    
    return 0;
}