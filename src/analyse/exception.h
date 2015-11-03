#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>

class Exception {
public:
  std::string errorMessage;
  Exception(std::string errorMessage) : errorMessage(errorMessage) {}
  void throwError() const {
    std::cout << errorMessage << std::endl;
    exit(1);
  }
};

#endif
