#include <iostream>
#include <unordered_map>
#include <string>

#include "command.h"

namespace COMMAND {
    std::unordered_map<std::string, std::string> analyse(int argc, const char* argv[]) {
        std::unordered_map<std::string, std::string> args;
        args.insert(std::make_pair("rootpath", std::string(argv[0])));
        for (size_t i = 1; i < argc; ++i) {
            std::string arg = std::string(argv[i]);
            int argLength = arg.length();
            if (arg.substr(0, 2) == "--") {
                args.insert(std::make_pair(arg.substr(2), "true"));
            } else if (arg.substr(argLength - 4, argLength) == ".sql") {
                args.insert(std::make_pair("file", arg));
            } else {
                cout << "Error: Invaild file type." << endl;
                exit(1);
            }
        }
        return args;
    }
}
