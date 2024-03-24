#ifndef LSP_SERVER_H_
#define LSP_SERVER_H_

#include <iostream>
#include <fstream>
#include <string>
#include "json.h"
#include "lexer.h"
#include "parser.h"
#include "json.h"
#include "json_parser.h"
#include "json_value.h"

class LspError : public exception
{
private:
  std::string msg;
public:
  bool eof = false;
  LspError(const std::string& msg,bool e = false)
  {
    this->msg = msg;
    this->eof = e;
  }
  const char* what() const noexcept
  {
    return msg.c_str();
  }
};

class LspServer
{
private:
    Lexer lexer;
    Parser parser;
    std::unordered_map<string,ProgramInfo> files;//current active files with uri as key and content as value
    std::ofstream out; //log file for debugging
    std::string header_name();
    std::string header_value();
    std::string get_payload_string();
    JsonObject* get_payload();
    void log(std::string);
    //functions to handle each type of request
    void initialize(JsonObject&);
    void initialized(JsonObject&);
    void cancel_request(JsonObject&);
    void did_change(JsonObject&);
    void did_close(JsonObject&);
    void did_open(JsonObject&);
    void did_save(JsonObject&);
public:
    LspServer();
    void run();
};

#endif
