#include"Parser.h"
#include<iostream>
#include <fstream>

using namespace std;

enum Op{
	OP_AND,
	OP_OR,
	OP_NOT,
	OP_IMPLY,
};

struct TreeNode{
    Op op;
    TreeNode* left;
    int ls, le;
    TreeNode* right;
    int rs, re;
}

FolParser::FolParser(vector<string> folkb, vector<string> folq){
    
    cout<<"parsing"<<endl;
    
    int lines_kb = str
}

FolParser::~FolParser(){

}

KB FolParser::getKB(){
    return kb;
}

vector<Clause> FolParser::getQuery(){
    return queries;
}

bool FolParser::parse(string s){
    int leftsemi = 0;
    int len = s.size();
    for(int i = 1; i< s.size(); ++i){
        if()
    }
    return false;
}