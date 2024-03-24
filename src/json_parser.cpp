#include "json_parser.h"
#include "json_value.h"
#include <exception>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include "json.h"
#include "utility.h"
using namespace std;



enum JsonTokenType
{
    STR=1,
    FLOAT=2,
    NULLVAL=3,
    COMMA=4,
    LCB=5,
    RCB=6,
    LP=7,
    RP=8,
    LSB=9,
    RSB=10,
    COL=11,
    NUM=12,
    BOOL=13

};
const char* StrJsonTokens[] = {
    "STR",
    "FLOAT",
    "NULLVAL",
    "COMMA",
    "LCB",
    "RCB",
    "LP",
    "RP",
    "LSB",
    "RSB",
    "COL",
    "NUM",
    "BOOL"
};
struct JsonToken
{
  JsonTokenType type;
  string content;
  JsonToken(JsonTokenType t,string cont)
  {
    type =t;
    content = cont;
  }
};
vector<JsonToken> tokenize(const string& str,bool& hadErr,string& msg)
{
  std::vector<JsonToken> tokens;
  hadErr = true;
  size_t l = str.length();
  size_t i = 0;
  int line_num = 1;
  int line_begin = 0;
  while(i<l)
  {
    char c = str[i];
    string cont;
    if(c=='"')
    {
      i+=1;
      int j = i;
      bool escaped = false;
      bool terminated = false;
      while(j<l)
      {
          if(str[j]=='"')
          {
              if(!escaped)
              {
                terminated = true;
                break;
              }
              cont+=str[j];
              escaped = false;
          }
          else if(str[j]=='\\')
          {
            if(escaped)
            {
              cont+="\\";
              escaped = false;
            }
            else
              escaped = true;
          }
          else
          {
            if(escaped)
            {
              if(str[j]=='n')
                cont+='\n';
              else if(str[j]=='r')
                cont+='\r';
              else if(str[j] == 't')
                cont+='\t';
              else if(str[j]=='b')
                cont+='\b';
              else if(str[j] == 'u')//unicode is not supported atleast not yet
                cont+="\\u";
              else
              {
                msg = (string)"Unknown escape char \\"+str[j];
                return tokens;
              }  
              escaped = false;            
            }
            else
              cont+=str[j];
          }
          j+=1;
      }
      if(!terminated)
      {
          msg = "Non terminated string!";
          return tokens;
      }
      if(escaped)
      {
          msg = "Non terminated escape char!";
          return tokens;
      }
      
      tokens.push_back(JsonToken(STR,cont));
      i = j;
    }
    else if(isdigit(c))
    {
        int j = i;
        bool decimal = false;
        while(j<l)
        {
          if(str[j]=='.' && !decimal)
          {
            decimal = true;
          }
          else if(!isdigit(str[j]))
          {
              break;
          }
          j+=1;
        }
        if(str[j-1]=='.') // floats like 2. are not considered valid it must be 2.0
        {
            msg = "Invalid float format";
            return tokens;
        }
        if(decimal)
          tokens.push_back(JsonToken(FLOAT,str.substr(i,j-i)));                  
        else
          tokens.push_back(JsonToken(NUM,str.substr(i,j-i)));
        i = j;
        continue;
    }
    else if(isalpha(c))
    {
        int j = i;
        while(j<l)
        {
          if(!isalpha(str[j]))
            break;
          j+=1;
        }
        string id = str.substr(i,j-i);
        if(id=="true" || id=="false")
        {
          tokens.push_back(JsonToken(BOOL,id));
        }
        else if(id=="null")
        {
            tokens.push_back(JsonToken(NULLVAL,id));
        }
        else
        {
           msg = "unknown keyword "+id +" at "+to_string(line_num)+":"+to_string(i-line_begin);
           return tokens;
        }
        i = j;
        continue;
    }
    else if(c==',')
    {
        tokens.push_back(JsonToken(COMMA,","));
    }
    else if(c=='[')
    {
        tokens.push_back(JsonToken(LSB,"["));
    }
    else if(c==']')
    {
        tokens.push_back(JsonToken(RSB,"]"));
    }
    else if(c=='{')
    {
        tokens.push_back(JsonToken(LCB,"{"));
    }
    else if(c=='}')
    {
        tokens.push_back(JsonToken(RCB,"}"));
    }
    else if(c==':')
    {
        tokens.push_back(JsonToken(COL,":"));
    }
    else if(c == '\n')
    {
      line_num++;
      line_begin = i+1;
    }
    else if(c==' ' || c=='\t'  || c=='\r')
    ;
    else
    {
       msg = "Unknown character ";
       msg += c;
       return tokens;
    }
    i+=1;
  }
  hadErr = false;
  return tokens;
}
bool isValTok(JsonToken t)
{
  if(t.type == NULLVAL || t.type == STR || t.type == FLOAT  || t.type == NUM || t.type==BOOL)
    return true;
  return false;
}


JsonValue TokToJObj(JsonToken t)
{
  JsonValue nil;
  nil.type = JsonValueType::JSON_NULL;
  if(t.type == NULLVAL)
    return nil;
  else if(t.type == STR)
    return JsonValue(t.content);
  else if(t.type == FLOAT)
    return JsonValue(atof(t.content.c_str()));
  else if(t.type == NUM)
    return JsonValue((int64_t)atoll(t.content.c_str()));
  else if(t.type == BOOL)
    return JsonValue((t.content == "true") ? true : false);
  return nil;
}

int match_lsb(int start,int end,vector<JsonToken>& tokens)
{
    int I = 0;
    for(int i=start;i<=end;i++)
    {
        if(tokens[i].type==LSB)
            I+=1;
        else if(tokens[i].type==RSB)
        {
            I-=1;
            if(I==0)
              return i;
        }
    }
    return -1;
}
int match_lcb(int start,int end,vector<JsonToken>& tokens)
{
    int I = 0;
    for(int i=start;i<=end;i++)
    {
        if(tokens[i].type==LCB)
            I+=1;
        else if(tokens[i].type==RCB)
        {
            I-=1;
            if(I==0)
              return i;
        }
    }
    return -1;
}
JsonObject* ObjFromJsonTokens(vector<JsonToken>&,int,int,bool&,string&);

vector<JsonValue>* ArrayFromJsonTokens(std::vector<JsonToken>& tokens,int l,int h,bool& err,string& msg)
{
  err = true;
  vector<JsonValue>* M = new vector<JsonValue>;
  if(tokens[l].type != LSB || tokens[h].type!=RSB)
    return M;
  for(int k=l+1;k<h;k+=1)
  {
    if(isValTok(tokens[k]))
    {
      M->push_back(TokToJObj(tokens[k]));
      k+=1;
    }
    else if(tokens[k].type == LCB) //subobject
    {
      int r = match_lcb(k,h-1,tokens);
      if(r == -1)
      {
        msg = "Unmatched curly bracket at "+to_string(k);
        return M;
      }
      JsonValue subobj(ObjFromJsonTokens(tokens,k,r,err,msg));
      if(err)
        return M;
      err = true;
      k = r+1;
      M->push_back(subobj);
    }
    else if(tokens[k].type == LSB) //list
    {
      int r = match_lsb(k,h-1,tokens);
      if(r == -1)
      {
        msg = "Unmatched square bracket at "+to_string(k);
        return M;
      }
      JsonValue sublist(ArrayFromJsonTokens(tokens,k,r,err,msg));
      if(err)
      {
        return M;
      }
      err = true;
      k = r+1;
      M->push_back(sublist);
    }
    else
    {
      msg = "Unknown token type ";
      return M;
    }
    if(k != h && tokens[k].type!=COMMA)
    {
      msg = "Expected comma";
      return M;
    }
  }
  err = false;
  return M;
}

JsonObject* ObjFromJsonTokens(std::vector<JsonToken>& tokens,int l,int h,bool& err,string& msg)
{
  err = true;
  JsonObject* M = new JsonObject;
  if(tokens[l].type != LCB || tokens[h].type!=RCB)
  {
    msg = "SyntaxError";
    return M;
  }
  string key;
  for(int k=l+1;k<h;k+=1)
  {
    if(tokens[k].type!=JsonTokenType::STR)
    {
      msg = "Non String keys!";
      return M;
    }
    key = tokens[k].content;
    k+=1;
    if(k+1 >= h || tokens[k].type!=COL)
    {
      msg = "Expected ':' after key ";
      return M;
    }
    k+=1;
    //key is at tokens[k-2]
    //values begins at tokens[k]
    if(isValTok(tokens[k]))
    {
      (*M)[tokens[k-2].content] = TokToJObj(tokens[k]);
      k+=1;
    }
    else if(tokens[k].type == LCB) //subobject
    {
      int r = match_lcb(k,h-1,tokens);
      if(r == -1)
      {
        msg = "Unmatched at bracket at "+to_string(k);
        return M;
      }
      JsonValue subobj(ObjFromJsonTokens(tokens,k,r,err,msg));
      if(err)
        return M;
      err = true;
      k = r+1;    
      (*M)[key] = subobj;
    }
    else if(tokens[k].type == LSB) //list
    {
      int r = match_lsb(k,h-1,tokens);
      if(r == -1)
      {
        msg = "Unmatched square bracket at "+to_string(k);
        return M;
      }
      JsonValue subarr(ArrayFromJsonTokens(tokens,k,r,err,msg));
      if(err)
        return M;
      err = true;
      k = r+1;
      (*M)[key] = subarr;
    }
    else
    {
      msg = "Unknown value against key "+key;
      return M;
    }
    if(k != h && tokens[k].type!=COMMA)
    {
      msg = "Expected comma after key/value pair";
      return M;
    }
  }
  err = false;
  return M;
}
bool dumperror;
std::string dumperrmsg;
void JObjToStr(JsonValue& p,std::string& res)
{
  if(p.type == JsonValueType::INT)
    res+= to_string(p.l);
  else if(p.type == JsonValueType::DOUBLE)
    res+= to_string(p.d);
  else if(p.type == JsonValueType::STR)
    res+= "\""+unescape(p.asStr())+"\"";
  else if(p.type == JsonValueType::BOOL)
    res += (p.b) ? "true" : "false";
  else if(p.type == JsonValueType::JSON_NULL)
    res+="null";
  else if(p.type == JsonValueType::ARRAY)
  {
      res+="[";
      auto l = p.asArray();
      size_t len = l.size();
      size_t k = 0;
      for(size_t idx=0;idx<len;idx++)
      {
        auto e = l[idx];
        JObjToStr(e,res);
        if(dumperror)
          return;
        if(k++ != len-1)
          res+=",";
      }
      res += "]";
  }
  else if(p.type == JsonValueType::OBJ)
  {
      res+="{";
      auto d = p.asObject();
      size_t k = 0;
      for(auto e: d)
      {

        res += "\"" + e.first+ "\"";
        res += ": ";
        JObjToStr(e.second,res);
        if(dumperror)
          return;
        res+=",";
      }
      if(res.back() == ',')
        res.pop_back();
      res += "}";
  }
}
///

JsonObject* loads(const string& src)
{
  bool hadErr;
  string msg;
  vector<JsonToken> tokens = tokenize(src,hadErr,msg);
  if(hadErr)
    throw JsonError("Tokenization failed. "+msg);
  if(tokens.size() == 0)
    throw JsonError("Empty String. Unable to tokenize!");
  JsonObject* m = ObjFromJsonTokens(tokens,0,tokens.size()-1,hadErr,msg);
  if(hadErr)
    throw JsonError(msg);
  return m;
}
string dumps(const JsonObject& json)
{
  dumperrmsg = "";
  dumperror = false;
  string res;
  std::unordered_map<void*,bool> seen;
  JsonValue tmp(json);
  JObjToStr(tmp,res);
  if(dumperror)
    throw JsonError(dumperrmsg);
  return res;
}
