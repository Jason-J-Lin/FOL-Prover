#include"Parser.h"
#include<iostream>
#include <fstream>

using namespace std;

FolParser::FolParser(vector<string> folkb, vector<string> folq){
    ifstream t(filename);
    string str((istreambuf_iterator<char>(t)),
                 istreambuf_iterator<char>());
    t.close();
    cout<<str<<endl;
    
    int lines_kb = str
}

FolParser::~FolParser(){

}