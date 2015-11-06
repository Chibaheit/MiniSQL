#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

#include <unordered_map>
#include <string>

namespace COMMAND {
    // analyse the command
    std::unordered_map<std::string, std::string> analyse(int argc, const char* argv[]);
}

#endif
