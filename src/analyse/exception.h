#ifndef EXCEPTION_H
#define EXCEPTION_H

class Exception {
public:
  string errorMessage;
  Exception(string errorMessage) : errorMessage(errorMessage) {}
  void throwError() const {
    std::cout << errorMessage << std::endl;
    exit(1);
  }
};

#endif
