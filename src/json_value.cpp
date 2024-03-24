#include "json_value.h"
#include "json.h"
#include <string.h>
#include <iostream>

int64_t JsonValue::asInt()
{
  return l;
}
double JsonValue::asDouble()
{
  return d;
}
bool JsonValue::asBool()
{
  return b;
}
std::vector<JsonValue>& JsonValue::asArray()
{
  return *(std::vector<JsonValue>*)ptr;
}
std::string& JsonValue::asStr()
{
  return *(std::string*)ptr;
}
JsonObject& JsonValue::asObject()
{
  return *(JsonObject*)ptr;
}
JsonValue& JsonValue::operator[](const string& key) // works if underlying type is json object
{
  static JsonValue null;
  null.type = JsonValueType::JSON_NULL;
  if(this->type == JsonValueType::OBJ)
  {
    JsonObject* p = (JsonObject*)ptr;
    return (*p)[key];
  }
  return null;
}
JsonValue& JsonValue::operator[](size_t idx) // works if underlying type is json array
{
  static JsonValue null;
  null.type = JsonValueType::JSON_NULL;
  if(this->type == JsonValueType::ARRAY)
  {
    JsonArray* p = (JsonArray*)ptr;
    return (*p)[idx];
  }
  return null;
}

bool JsonValue::operator==(const JsonValue& rhs)const
{
  if(type != rhs.type)
    return false;
  if(type == JsonValueType::JSON_NULL)
    return true;
  else if(type == JsonValueType::ARRAY)
  {
    vector<JsonValue>& a = *(vector<JsonValue>*)ptr;
    vector<JsonValue>& b = *(vector<JsonValue>*)rhs.ptr;
    return a == b;
  }
  else if(type == JsonValueType::OBJ)
  {
    JsonObject& a = *(JsonObject*)ptr;
    JsonObject& b = *(JsonObject*)rhs.ptr;
    return a == b;
  }
  else if(type == JsonValueType::BOOL)
    return this->b == rhs.b;
  else if(type == JsonValueType::DOUBLE)
    return this->d == rhs.d;
  else if(type == JsonValueType::INT)
    return this->l == rhs.l;
  else if(type == JsonValueType::STR)
  {
    string& a = *(string*)ptr;
    string& b = *(string*)rhs.ptr;
    return a==b;
  }
  return false; //to silence warning
}
bool JsonValue::operator!=(const JsonValue& rhs) const
{
  return !(*this == rhs);
}

//Constructors
JsonValue::JsonValue()
{
  type = JsonValueType::JSON_NULL;
}
JsonValue::JsonValue(const JsonValue& rhs)
{
  this->type = rhs.type;
  switch(rhs.type)
  {
    case JsonValueType::JSON_NULL:
      return;
    case JsonValueType::INT:
    {
      this->l = rhs.l;
      break;
    }
    case JsonValueType::BOOL:
    {
      this->b = rhs.b;;
      break;
    }
    case JsonValueType::DOUBLE:
    {
      this->d = rhs.d;
      break;
    }
    case JsonValueType::STR:
    {
      std::string* p = new std::string(*(string*)rhs.ptr);
      this->ptr = (void*)p;
      break;
    }
    case JsonValueType::ARRAY:
    {
      JsonArray* p = new JsonArray(*(JsonArray*)rhs.ptr);
      this->ptr = (void*)p;
      break;
    }
    case JsonValueType::OBJ:
    {
      JsonObject* p = new JsonObject(*(JsonObject*)rhs.ptr);
      this->ptr = (void*)p;
      break;
    }    
  }
}
JsonValue& JsonValue::operator=(const JsonValue& rhs)
{
  if(&rhs == this)
    return *this;
  //Delete previous memory if any
  if(type == JsonValueType::ARRAY)
    delete (JsonArray*)ptr;
  else if(type == JsonValueType::STR)
    delete (std::string*)ptr;
  else if(type == JsonValueType::OBJ)
    delete (JsonObject*)ptr;

  this->type = rhs.type;
  switch(rhs.type)
  {
    case JsonValueType::JSON_NULL:
      return *this;
    case JsonValueType::INT:
    {
      this->l = rhs.l;
      break;
    }
    case JsonValueType::BOOL:
    {
      this->b = rhs.b;;
      break;
    }
    case JsonValueType::DOUBLE:
    {
      this->d = rhs.d;
      break;
    }
    case JsonValueType::STR:
    {
      std::string* p = new std::string(*(string*)rhs.ptr);
      this->ptr = (void*)p;
      break;
    }
    case JsonValueType::ARRAY:
    {
      JsonArray* p = new JsonArray(*(JsonArray*)rhs.ptr);
      this->ptr = (void*)p;
      break;
    }
    case JsonValueType::OBJ:
    {
      JsonObject* p = new JsonObject(*(JsonObject*)rhs.ptr);
      this->ptr = (void*)p;
      break;
    }    
  }
  return *this;
}
JsonValue::JsonValue(int64_t l)
{
  type = JsonValueType::INT;
  this->l = l;
}
JsonValue::JsonValue(double d)
{
  type = JsonValueType::DOUBLE;
  this->d = d;
}
JsonValue::JsonValue(bool b)
{
  type = JsonValueType::BOOL;
  this->b = b;
}
JsonValue::JsonValue(const string& str)
{
  string* p = new string(str);
  type = JsonValueType::STR;
  ptr = (void*)p;
}
JsonValue::JsonValue(JsonObject* obj)
{
  type = JsonValueType::OBJ;
  ptr = obj;
}
JsonValue::JsonValue(vector<JsonValue>* arr)
{
  type = JsonValueType::ARRAY;
  ptr = (void*)arr;
}
JsonValue::JsonValue(const JsonObject& obj)
{

  JsonObject* p = new JsonObject(obj);
  type = JsonValueType::OBJ;
  this->ptr = (void*)p;
}
JsonValue::JsonValue(const JsonArray& arr)
{
  type = JsonValueType::ARRAY;
  JsonArray* p = new JsonArray(arr);
  ptr = (void*)p;
}
JsonValue::~JsonValue()
{
  
  if(type == JsonValueType::ARRAY)
    delete (JsonArray*)ptr;
  else if(type == JsonValueType::STR)
    delete (std::string*)ptr;
  else if(type == JsonValueType::OBJ)
    delete (JsonObject*)ptr;
  
}

ostream& operator<<(ostream& out,const JsonValue& val)
{
  switch(val.type)
  {
    case JsonValueType::JSON_NULL:
    {
      out << "null";
      break;
    }
    case JsonValueType::INT:
    {
      out << val.l;
      break;
    }
    case JsonValueType::BOOL:
    {
      out << val.b;
      break;
    }
    case JsonValueType::DOUBLE:
    {
      out << val.d;
      break;
    }
    case JsonValueType::STR:
    {
      out << (*(string*)val.ptr);
      break;
    }
    case JsonValueType::ARRAY:
    {
      JsonArray& arr = *(JsonArray*)val.ptr;
      out<<"[";
      for(size_t i=0;i<arr.size();i++)
      {
        out << arr[i];
        if(i!=arr.size()-1)
          out<<",";
      }
      out<<"]";
      break;
    }
    case JsonValueType::OBJ:
    {
      JsonObject& obj = *(JsonObject*)val.ptr;
      size_t len = obj.size();
      size_t i =0;
      out<<"{";
      for(auto e: obj)
      {
        if(e.second.type == JsonValueType::STR)
          out << e.first <<": \""<<(*(string*)e.second.ptr)<<"\"";
        else
          out << e.first <<": "<<e.second;
        if(i!=len-1)
          out << ",";
        i++;
      }
      out<<"}";
      break;
    }    
  }
  return out; 
}