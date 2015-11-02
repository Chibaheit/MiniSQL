#ifndef IO_H
#define IO_H

#include <string>

// use history to save minisql script
class IO {
public:
    // read line and save to history
    bool readLine(std::string& chunk, std::string shell_prompt);

    int initialization();

    int saveInstruction();
};

#endif
