/*MIT License

Copyright (c) 2022 Shahryar Ahmad 

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/
#ifndef PARSER_H_
#define PARSER_H_
#include "utility.h"
#include "token.h"
#include "ast.h"
#include "programinfo.h"
#include "lexer.h"
#include <algorithm>
#include <exception>
#include <queue>
#include <unordered_map>

#ifdef _WIN32

#else
  #include <dirent.h>
  #include <unistd.h>
  #include <errno.h>
  #include <libgen.h>
#endif


Node* NewNode(NodeType type,string val="");
void stripNewlines(vector<Token>& tokens);
int matchRPRight(int k,const vector<Token>& tokens);
int findBeginList(int k,const vector<Token>& tokens);
int findLCBRight(int k,const vector<Token>& tokens);
int findToken(Token t,int start,const vector<Token>& tokens);
int matchRP(int k,const vector<Token> tokens);
int findEndList(int k,const vector<Token>& tokens);
int findRCB(int k,const vector<Token>& tokens);
void deleteAST(Node* ast);
void CopyAst(Node*& dest,Node* src);

// Function to print AST in tablular form
void printAST(Node* n,int spaces = 0);
int findTokenConsecutive(Token t,int start,const vector<Token>& tokens);

class ParseError : public exception
{
private:
  std::string msg;
public:
  size_t line;
  ParseError(const std::string& msg,size_t line_num)
  {
    this->msg = msg;
    this->line = line_num;
  }
  const char* what() const noexcept
  {
    return msg.c_str();
  }
};
//Not actually thrown
// the parser keeps stacking them so the program that called the parser can look'em
class MinorError
{
public:
  string type; //can be SyntaxError or NameError
  string msg;
  string filename;
  int64_t line;
  MinorError()
  {

  }
  MinorError(string type,string msg,string filename)
  {
    this->type = type;
    this->msg = msg;
    this->filename = filename;
  }
};
#define SymbolTable std::unordered_map<string,Node*>
class Parser
{
private:
  vector<Token> known_constants;
  vector<string> prefixes = {""};//for namespaces 
  std::unordered_map<string,vector<string>>* refGraph;
  SymbolTable globals;
  std::vector<SymbolTable> locals;

  string currSym;
  vector<string>* files;
  vector<string>* sources;
  string filename;
  size_t line_num = 1;
  int* num_of_constants;
  std::string aux;
  bool foundYield = false;
  //Context
  bool infunc;
  bool inclass;
  bool inloop;
  bool inif;
  bool inelif;
  bool inelse;
  bool intry;
  bool incatch;
  //in class method = inclass && infunc
  inline bool atGlobalLevel()
  {
    return (!infunc && !inclass && !inloop
     && !inif && !inelif && !inelse && !intry && !incatch);
  }
  inline bool isValidCtxForFunc()
  {
        return (!infunc  && !inloop
     && !inif && !inelif && !inelse && !intry && !incatch);
  }
  Node* findLocal(const std::string&);
  void parseError(string type,string msg); // for critcal errors that stop the parser from continuing
  Node* minorError(string type,string msg);
  bool addSymRef(string name);
  Node* parseExpr(const vector<Token>& tokens);
  Node* parseStmt(vector<Token> tokens);
public:
  vector<MinorError> minors;
  void init(string fname,ProgramInfo& p);
  Node* parse(const vector<Token>& tokens);
};
#endif
