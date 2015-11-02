#include <iostream>

#include "command.h"
#include "exception.h"

using namespace std;

void Command::analyse(int argc, const char* argv[]) {
    args.insert(make_pair("rootpath", string(argv[0])));
    for (int i = 1; i < argc; ++i) {
        string arg = string(argv[i]);
        int argLength = arg.length();
        if (arg.substr(0, 2) == "--") {
            args.insert(make_pair(arg.substr(2), "true"));
        } else if (arg.substr(argLength - 4, argLength) == ".sql") {
            args.insert(make_pair("file", arg));
        } else {
            Exception e = Exception("Error: Invaild argument.");
            e.throwError();
        }
    }
}

void Command::printArguments() const {
    for (auto &u: args) {
        cout << u.first << " " << u.second << endl;
    }
}
