#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

#include <unordered_map>

using namespace std;

class Command {
public:
    unordered_map<string, string> args;
    // analyse the command
    void analyse(int argc, const char* argv[]);

    // print with command
    void printArguments() const;
};

#endif
