
#ifndef JSON_OBJECT_H_
#define JSON_OBJECT_H_
#include <string>
#include <cstdint>
#include <vector>
#include "json.h"

using namespace std;


enum class JsonValueType
{
  INT=0,
  STR=1,
  OBJ=2,
  JSON_NULL=3,
  DOUBLE=4,
  BOOL=5,
  ARRAY=6
};

struct JsonValue // Variant object to store json values
{
  union
  {
    int64_t l;
    void* ptr;
    bool b;
    double d;
  };
  JsonValueType type;

  JsonValue();
  JsonValue(const JsonValue&);
  JsonValue(int64_t);
  JsonValue(double);
  JsonValue(bool);
  JsonValue(const std::string&); // makes deep copy
  JsonValue(JsonObject*); // makes shallow copy
  JsonValue(std::vector<JsonValue>*); // makes shallow copy
  JsonValue(const JsonObject&); // makes deep copy
  JsonValue(const std::vector<JsonValue>&); // makes deep copy
  JsonValue& operator=(const JsonValue& rhs);
  JsonValue& operator[](const string& key); // if the underlying value is a nested object, then this operator can be used
  //it is the shorthand for obj.asObj()[key]
  JsonValue& operator[](size_t); // if the underlying value is an array, then this operator can be used
  //it is the shorthand for obj.asArray()[key]
  
  bool operator==(const JsonValue&)const;
  bool operator!=(const JsonValue&)const;
  friend ostream& operator<<(ostream&,const JsonValue&);
  
  ~JsonValue();
  
  int64_t asInt();
  double asDouble();
  bool asBool();
  std::vector<JsonValue>& asArray();
  std::string& asStr();
  JsonObject& asObject();
  
  
};



#endif