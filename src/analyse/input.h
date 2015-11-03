#ifndef INPUT_H
#define INPUT_H

#include <string>

// use history to save minisql script
namespace INPUT {
  bool readLine(std::string& chunk, std::string shell_prompt);

  int initialization();

  int saveCommand();
}

#endif
