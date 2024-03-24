#ifndef JSON_H_
#define JSON_H_


#include <unordered_map>
#include <vector>

struct JsonValue;
#define JsonObject std::unordered_map<std::string,JsonValue>
#define JsonArray std::vector<JsonValue>

#endif