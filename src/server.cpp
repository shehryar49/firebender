#define DEBUG_LSP
#include "server.h"
#include "json.h"

using namespace std;

LspServer::LspServer()
{
    #ifdef _WIN32
      out.open("C:\\zuko\\lsp\\out.txt");
    #else
      out.open("/opt/zuko/lsp/out.txt");
    #endif
}
string LspServer::header_name()
{
    signed char ch;
    string val;
    while((ch = cin.get()))
    {
        if(ch == EOF) //premature EOF
        {
            throw LspError("Early EOF",true);
        }
        else if(ch == ':')
            break;
        else if(!isalpha(ch) && ch!='-')
            throw LspError("Illegal character in header name");
        val += ch;
    }
    //read space
    ch = cin.get();
    if(ch != ' ')
      return "";
    return val;
}
string LspServer::header_value()
{
    signed char ch;
    string val;
    while((ch = cin.get()))
    {
        if(ch == EOF) //premature EOF
            throw LspError( "Early EOF",true);
        else if(ch == '\r')
            break;
        val += ch;
    }
    //consume LF
    if(cin.get() != '\n') // ERROR
        throw LspError("No valid CRLF after header value");
    return val;
}
string LspServer::get_payload_string() //reads exactly one payload from stream
{
    signed char ch;
    string result; // to store content
    string header;
    string value;
    bool content_read = false;
    bool len_set = false;
    size_t len = 0;

    while(!content_read)
    {
      ch = cin.get();
      if(ch == '\r') // content has begun
      {
        ch = cin.get();
        if(ch != '\n')
          throw LspError("Expected newline after carriage return!");
        //CRLF was consumed
        if(!len_set)
          throw LspError("Length header not found.");

        while(len != 0)
        {
          ch = cin.get();
          if(ch == EOF)
            throw LspError("Early EOF",true);
          result += ch;
          len --;
        }
        break; 
      }
      else 
      {
        cin.unget();
        header = header_name();
        value = header_value();
        if(header == "Content-Length")
        {
          len = atoll(value.c_str());
          len_set = true;
        }
        else if(header == "Content-Type")
         ;
        else
        {
          throw LspError("Unknown header");
        }
      }
    }
    return result;
}
JsonObject* LspServer::get_payload()
{
    string str;
    try 
    {
        str = get_payload_string();
    }
    catch (const LspError& e)
    {
        if(e.eof)
        {
            out << "Received EOF. Exiting" << endl;
            exit(0);
        }
        out<<e.what()<<endl;
        return nullptr;
    } 
    
    log(str);
    
    JsonObject* payload;
    try
    {
        payload = loads(str);
    }
    catch(const JsonError& e)
    {
        log(e.what());
        return nullptr;
    }
    return payload;
}
string timestamp()
{
  time_t now = time(NULL);
  string result = ctime(&now);
  result.pop_back();
  result = "[" + result + "]";
  return result;
}
void LspServer::log(string msg)
{
  #ifdef DEBUG_LSP
    if(out)
        out << timestamp() << "  " << msg << endl;
  #endif
}

// The juicy part
void LspServer::initialize(JsonObject& json)
{
    char response[] = {0x7B,0x0A,0x22,0x6A,0x73,0x6F,0x6E,0x72,0x70,0x63,0x22,0x3A,0x20,0x22,0x32,0x2E,0x30,0x22,0x2C,0x0A,0x20,0x20,0x22,0x69,0x64,0x22,0x3A,0x20,0x30,0x2C,0x0A,0x20,0x20,0x22,0x72,0x65,0x73,0x75,0x6C,0x74,0x22,0x3A,0x20,0x7B,0x0A,0x20,0x20,0x20,0x20,0x22,0x63,0x61,0x70,0x61,0x62,0x69,0x6C,0x69,0x74,0x69,0x65,0x73,0x22,0x3A,0x20,0x7B,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x22,0x74,0x65,0x78,0x74,0x44,0x6F,0x63,0x75,0x6D,0x65,0x6E,0x74,0x53,0x79,0x6E,0x63,0x22,0x3A,0x20,0x31,0x2C,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x22,0x63,0x6F,0x6D,0x70,0x6C,0x65,0x74,0x69,0x6F,0x6E,0x50,0x72,0x6F,0x76,0x69,0x64,0x65,0x72,0x22,0x3A,0x20,0x7B,0x7D,0x2C,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x22,0x73,0x69,0x67,0x6E,0x61,0x74,0x75,0x72,0x65,0x48,0x65,0x6C,0x70,0x50,0x72,0x6F,0x76,0x69,0x64,0x65,0x72,0x22,0x3A,0x20,0x7B,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x22,0x74,0x72,0x69,0x67,0x67,0x65,0x72,0x43,0x68,0x61,0x72,0x61,0x63,0x74,0x65,0x72,0x73,0x22,0x3A,0x20,0x5B,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x22,0x28,0x22,0x2C,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x22,0x2C,0x22,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x5D,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x7D,0x0A,0x20,0x20,0x20,0x20,0x7D,0x0A,0x20,0x20,0x7D,0x0A,0x7D,0x00};
    log("Received init request");
    log(response);
    cout <<"Content-Length: 250\r\n\r\n";
    cout << response ;
}
void LspServer::initialized(JsonObject& json)
{
    //do nothing
}
void LspServer::cancel_request(JsonObject& json)
{
    //do nothing
}
//
void applyRangeChange(string& text,int64_t startline,int64_t startcol,int64_t endline,int64_t endcol,string newText)
{
  size_t i = 0;
  size_t line = 0;
  size_t len = text.length();
  size_t startidx = 0;
  size_t endidx = 0;
  bool found1 = false;
  bool found2 = false;
  while((!found1 || !found2) && i < len)
  {
    if(line == startline && !found1)
    {   
      startidx = i;
      found1 = true;
    }
    else if(line == endline && !found2)
    {
      endidx = i;
      found2 = true;
    }
    if(text[i] == '\n')
      line++;
    i++;
    
  }
  startidx += startcol+1;
  endidx += endcol - 1;

  if(startidx <= endidx)
    text.replace(startidx,endidx-startidx+1,newText);
}

//
void LspServer::did_change(JsonObject& json)
{
    string uri = json["params"]["textDocument"]["uri"].asStr();
    int version = json["params"]["textDocument"]["version"].asInt();
    if(files[uri].version > version)
        return;
    JsonArray& changes = json["params"]["contentChanges"].asArray();
    for(auto change: changes)
    {
        JsonObject& chg = change.asObject();
        if(chg.find("range") != chg.end()) // range specfied
        {
            //apply changes
            string new_text = chg["text"].asStr();
            JsonObject& range = chg["range"].asObject();
            JsonObject& start = range["start"].asObject();
            JsonObject& end = range["end"].asObject();
            log( "applying range changes");
            applyRangeChange(files[uri].sources[0],start["line"].asInt(),start["character"].asInt(),end["line"].asInt(),end["character"].asInt(),new_text);
            log( files[uri].sources[0]);
        }
        else if(chg.find("text")!=chg.end())
        {
            log( "Replacing with new text" );
            string text =  chg["text"].asStr();
            log(text);
            files[uri].reset();
            files[uri].sources.push_back(text);
            files[uri].version = version;
        }
    }
    JsonObject response;
    response["jsonrpc"] = (string)"2.0";
    response["method"] = (string)"textDocument/publishDiagnostics";
    JsonObject params;
    params["uri"] = uri;
    JsonArray diagnostics;
    try 
    {
        auto tokens = lexer.generateTokens(uri,files[uri].sources[0],false);
        if(lexer.hadErr)
          throw ParseError(lexer.errmsg,lexer.line_num);
        parser.init(uri,files[uri]);
        Node* ast = parser.parse(tokens);
        for(auto err: parser.minors)
        {
            JsonObject diagnostic;
            JsonObject range;
            JsonObject start;
            JsonObject end;
            start["line"] = err.line - 1;
            start["character"] = (int64_t)0;
            end["line"] = err.line;
            end["character"] = (int64_t)0;
            range["start"] = start;
            range["end"] = end;
            diagnostic["code"] = (int64_t)6969;
            diagnostic["source"] = (string)"Zuko LSP";
            diagnostic["severity"] = (int64_t)1;
            diagnostic["message"] = err.msg;
            diagnostic["range"] = range;
            diagnostics.push_back(diagnostic);
          }
          deleteAST(ast);
    }
    catch (const ParseError& err) 
    {
        JsonObject diagnostic;
        JsonObject range;
        JsonObject start;
        JsonObject end;
        start["line"] = (int64_t)err.line - 1;
        start["character"] = (int64_t)0;
        end["line"] = (int64_t)err.line;
        end["character"] = (int64_t)0;
        range["start"] = start;
        range["end"] = end;
        diagnostic["code"] = (int64_t)6969;
        diagnostic["source"] = (string)"Zuko LSP";
        diagnostic["severity"] = (int64_t)1;
        diagnostic["message"] = (string)err.what();
        diagnostic["range"] = range;
        diagnostics.push_back(diagnostic);
    }  
    log( "Sending diagnostics on didChange");
    params["diagnostics"] = diagnostics;
    response["params"] = params;
    string content = dumps(response);
    cout << "Content-Length: "<<content.length()<<"\r\n\r\n";
    log( content );
    cout<<content;
}
//
void LspServer::did_close(JsonObject& json)
{
    string uri = json["params"]["textDocument"]["uri"].asStr();
    files.erase(uri);
    //send empty diagnostics
    JsonObject response;
    response["jsonrpc"] = (string)"2.0";
    response["method"] = (string)"textDocument/publishDiagnostics";

    JsonObject params;
    params["uri"] = uri;

    JsonArray diagnostics;
    log(  "Sending diagnostics on didClose");
    params["diagnostics"] = diagnostics;
    response["params"] = params;
    string content = dumps(response);
    cout << "Content-Length: "<<content.length()<<"\r\n\r\n";
    log( content );
    cout<<content;
}
void LspServer::did_open(JsonObject& json)
{
    auto textDocument = json["params"]["textDocument"];
    auto uri = textDocument["uri"].asStr();
    auto text = textDocument["text"].asStr();
    log("Opened file "+uri);
    log(text);

    files[uri] = ProgramInfo(uri,text,0);
    JsonObject response;
    response["jsonrpc"] = (string)"2.0";
    response["method"] = (string)"textDocument/publishDiagnostics";

    JsonObject params;
    params["uri"] = uri;

    JsonArray diagnostics;

    try
    {
        auto tokens = lexer.generateTokens(uri,text);
        if(lexer.hadErr)
            throw ParseError(lexer.errmsg,lexer.line_num);
        parser.init(uri,files[uri]);
        Node* ast = parser.parse(tokens);
        for(auto err: parser.minors)
        {
            JsonObject diagnostic;
            JsonObject range;
            JsonObject start;
            JsonObject end;
            start["line"] = err.line-1;
            start["character"] = (int64_t)0;
            end["line"] = err.line;
            end["character"] = (int64_t)0;
            range["start"] = start;
            range["end"] = end;
            diagnostic["code"] = (int64_t)6969;
            diagnostic["source"] = (string)"Zuko LSP";
            diagnostic["severity"] = (int64_t)1;
            diagnostic["message"] = err.msg;
            diagnostic["range"] = range;
            diagnostics.push_back(diagnostic);
        }
        deleteAST(ast);
    }
    catch(const ParseError& err)
    {
        JsonObject diagnostic;
        JsonObject range;
        JsonObject start;
        JsonObject end;
        start["line"] = (int64_t)err.line-1;
        start["character"] = (int64_t)0;
        end["line"] = (int64_t)err.line;
        end["character"] = (int64_t)0;
        range["start"] = start;
        range["end"] = end;
        diagnostic["code"] = (int64_t)6969;
        diagnostic["source"] = (string)"Zuko LSP";
        diagnostic["severity"] = (int64_t)1;
        diagnostic["message"] = (string)err.what();
        diagnostic["range"] = range;
        diagnostics.push_back(diagnostic);
    }
    
    log("Sending diagnostics on didOpen");
    params["diagnostics"] = diagnostics;
    response["params"] = params;
    string content = dumps(response);
    log( content );
    cout << "Content-Length: "<<content.length()<<"\r\n\r\n";
    cout<<content;
}
void LspServer::run()
{
    log("Started Zuko LSP Server");
    while(true)
    {
        log("Receiving payload");
        JsonObject* payload = get_payload();

        if(!payload)
            continue;
        auto json = *payload;
        if(json["method"].asStr() == "initialize")
            initialize(json);
        else if(json["method"].asStr() == "initialized")//do nothing, it's a notification
            initialized(json);
        else if(json["method"].asStr() == "$/cancelRequest") // irrelevant to this implementation
            cancel_request(json);
        else if(json["method"].asStr() == "textDocument/didChange")
            did_change(json);
        else if(json["method"].asStr() == "textDocument/didClose")
            did_close(json);
        else if(json["method"].asStr() == "textDocument/didOpen")
            did_open(json);
        else if(json["method"].asStr() == "shutdown")
        {
            log( "Received shutdown request");
            delete payload;
            break;
        }
        delete payload;
    }
}