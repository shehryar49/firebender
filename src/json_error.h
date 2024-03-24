#ifndef JSON_ERROR
#define JSON_ERROR

#include <exception>
#include <string>

class JsonError : public std::exception
{
private:
  std::string msg;
public:
  JsonError(const std::string& msg)
  {
    this->msg = msg;
  }
  const char* what() const noexcept
  {
    return msg.c_str();
  }
};

#endif