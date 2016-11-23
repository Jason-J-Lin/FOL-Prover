#include"Parser.h"
#include<iostream>
#include <fstream>
#include <algorithm>

using namespace std;

FolParser::FolParser(vector<string> folkb, vector<string> folq){
    
    for(string s : folkb){
        s.erase(remove(s.begin(),s.end(),' '), s.end());
        cout<<"parsing "<<s<<endl;
        TreeNode* head = new TreeNode();
        head->r = s.size()-1;
        parse(s, head);
        convert(head);
        delete head;
    }
    for(string s : folq){
        TreeNode* head = new TreeNode();
        head->r = s.size()-1;
        parse(s, head);
        convert(head);
        delete head;
    }
}

FolParser::~FolParser(){

}

KB FolParser::getKB(){
    return kb;
}

vector<Clause> FolParser::getQuery(){
    return queries;
}

// set a node first and use it as a container
// 
// tn ->  "string"    <- check if can be splitted 
//          /  \      <- create nodes for substring now
//     "sub1"  "sub2"
// 
bool FolParser::parse(string &s, TreeNode* tn){
    // need a judgement here? whether to continue.
    int leftsemi = 0;
    /* five possible parsing:
        not
        implication
        or
        and
        default!
        ----------------------
        start with not (only need to look at the first char)
    */ 
    if(s.at(tn->l+1)=='~'){
        #ifdef DEBUG
        cout<<"not "<<tn->l<<" "<<tn->r<<endl;
        #endif
        tn->op = OP_NOT;
        TreeNode* tn1 = new TreeNode();             //creating new node
        tn->left = tn1;
        tn->right = NULL;
        tn1->l = tn->l+2;
        tn1->r = tn->r-1;
        parse(s, tn1);
        return true;
    }
    for(int i = tn->l+1; i< tn->r-1; ++i){
        if(s[i] == ' ') continue;   // or to eliminate all space together?
        if(leftsemi == 0){
            if(s[i] == '&'){
                #ifdef DEBUG
                cout<<"and"<<endl;
                #endif
                tn->op = OP_AND;
                TreeNode *tn1 = new TreeNode(), *tn2 = new TreeNode();             //creating new node
                tn->left = tn1;
                tn->right = tn2;
                tn1->l = tn->l+1;
                tn1->r = i-1;
                tn2->l = i+1;
                tn2->r = tn->r-1;
                parse(s, tn1);
                parse(s, tn2);
                return true;
            }else if(s[i] == '|'){
                #ifdef DEBUG
                cout<<"or"<<endl;
                #endif
                tn->op = OP_OR;
                TreeNode *tn1 = new TreeNode(), *tn2 = new TreeNode();             //creating new node
                tn->left = tn1;
                tn->right = tn2;
                tn1->l = tn->l+1;
                tn1->r = i-1;
                tn2->l = i+1;
                tn2->r = tn->r-1;
                parse(s, tn1);
                parse(s, tn2);
                return true;
            }else if(s[i] == '='){
                #ifdef DEBUG
                cout<<"imply"<<endl;
                #endif
                tn->op = OP_IMPLY;
                TreeNode *tn1 = new TreeNode(), *tn2 = new TreeNode();
                tn->left = tn1;
                tn->right = tn2;
                tn1->l = tn->l+1;
                tn1->r = i-1;
                tn2->l = i+2;
                tn2->r = tn->r-1;
                parse(s, tn1);
                parse(s, tn2);
                return true;
            }
        }
        if(s[i] == '('){
            leftsemi++;
        }else if(s[i] == ')'){
            leftsemi--;
        }
    }
    tn->op = OP_DEF;
    tn->left = NULL;
    tn->right = NULL;
    tn->context = s.substr(tn->l, tn->r - tn->l+1);
    #ifdef DEBUG
    cout<<"def: "<<tn->context<<endl;
    #endif
    return true;
}

bool FolParser::convert(TreeNode* tn){
    #ifdef DEBUG
    cout<<"start converting"<<endl;
    #endif
    impElim(tn);
    notInwd(tn);
    andDstb(tn);
    return true;
}

void FolParser::impElim(TreeNode* tn){
    if(!tn) return;
    if(tn->op == OP_IMPLY){
        #ifdef DEBUG
        cout<<"imply eliminated"<<endl;
        #endif
        TreeNode* tempNot = new TreeNode();
        tempNot->left = tn->left;
        tempNot->op = OP_NOT;
        tn->op = OP_OR;
        tn->left = tempNot;
    }
    impElim(tn->left);
    impElim(tn->right);
}

void FolParser::notInwd(TreeNode* tn){
    if(!tn) return;
    if(tn->op == OP_NOT){
        if(tn->left->op == OP_NOT){
            #ifdef DEBUG
            cout<<"double not eliminated"<<endl;
            #endif
            TreeNode *tdel = tn->left;                  // Node Creating
            delete tdel;   // Many nodes to be deleted!!
            tn = tn->left->left;
        }else if(tn->left->op == OP_AND){
            #ifdef DEBUG
            cout<<"not into and"<<endl;
            #endif
            TreeNode *tempNotLeft = new TreeNode(), *tempNotRight = new TreeNode();       // Node Creating
            tempNotLeft->left = tn->left->left;
            tempNotRight->left = tn->left->right;
            tempNotLeft->op = OP_NOT;
            tempNotRight->op = OP_NOT;
            tn->op = OP_OR;
            tn->left = tempNotLeft;
            tn->right = tempNotRight;
        }else if(tn->left->op == OP_OR){
            #ifdef DEBUG
            cout<<"not into or"<<endl;
            #endif
            TreeNode *tempNotLeft = new TreeNode(), *tempNotRight = new TreeNode();       // Node Creating
            tempNotLeft->left = tn->left->left;
            tempNotRight->left = tn->left->right;
            tempNotLeft->op = OP_NOT;
            tempNotRight->op = OP_NOT;
            tn->op = OP_AND;
            tn->left = tempNotLeft;
            tn->right = tempNotRight;            
        }else if(tn->left->op == OP_DEF){  // must be default
            #ifdef DEBUG
            cout<<"not into def"<<endl;
            #endif
            tn->context = tn->left->context;
            delete tn->left;
            tn->left = NULL;
            return;
        }else{
            cout<<"not Inward parsing error!"<<endl;
            return;
        }
        notInwd(tn);
    }else{
        notInwd(tn->left);
        notInwd(tn->right);
    }
}

void FolParser::andDstb(TreeNode* tn){
    if(!tn) return;
    if(tn->op == OP_OR){
        if(tn->left->op == OP_AND){
            #ifdef DEBUG
            cout<<"left and distributed"<<endl;
            #endif
            tn->op = OP_AND;
            tn->left->op = OP_OR;
            TreeNode* tempRight = new TreeNode();                        // Node Creating
            tempRight->op = OP_OR;
            tempRight->left = tn->left->right;
            tempRight->right = tn->right;
            tn->left->right = tn->right;
            tn->right = tempRight;
        }else if(tn->right->op == OP_AND){
            #ifdef DEBUG
            cout<<"left and distributed"<<endl;
            #endif
            tn->op = OP_AND;
            tn->right->op = OP_OR;
            TreeNode* tempLeft = new TreeNode();                         // Node Creating
            tempLeft->op = OP_OR;
            tempLeft->left = tn->left;
            tempLeft->right = tn->right->left;
            tn->right->left = tn->left;
            tn->left = tempLeft;
        }
    }
    andDstb(tn->left);                                  // Unsure about this logic
    andDstb(tn->right);
}


// For testing
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
    // KB kb = parser->getKB();
    // vector<Clause> queries = parser->getQuery();
    

    return 0;
}