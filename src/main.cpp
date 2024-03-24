#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <string.h>
#include "json.h"
#include "json_value.h"
#include "json_parser.h"
#include "json_error.h"
#include "parser.h"
#include "lexer.h"
#include "programinfo.h"
#include "server.h"
#include <ctime>

using namespace std;


int main()
{
  LspServer server;
  server.run();
  return 0;  
}