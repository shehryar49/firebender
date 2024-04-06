cmake_minimum_required(VERSION 3.16.3 FATAL_ERROR)
project(firebender)
set(CMAKE_CXX_STANDARD 11)
add_executable(firebender src/main.cpp src/json.h src/json_value.cpp src/json_value.h src/json_parser.h src/json_parser.cpp src/json_error.h src/parser.h src/parser.cpp src/lexer.h src/lexer.cpp src/convo.h src/convo.cpp src/overflow.h src/overflow.cpp src/utility.h src/utility.cpp src/server.h src/server.cpp)
