#include <iostream>
#include <cstdio>
#include <cassert>

#include "./src/interpreter/command.h"
#include "./src/interpreter/help.h"
#include "./src/api/repl.h"

using namespace std;

const string VERSION = "0.0.3";
int defaultNumBlocks = 2, defaultBlockSize = 4096;

int main(int argc, const char* argv[]) {
    auto args = COMMAND::analyse(argc, argv);
    // it means ./minisql
    if (argc == 1) {
        REPL::repl(VERSION);
    }

    if (argc == 2) {
        if (args.find("help") != args.end()) {
            HELP::help();
            return 0;
        } else if (args.find("file") != args.end()) {
            REPL::file(args["file"]);
        } else {
            cout << "Error: Invalid argument." << endl;
        }
    }
    return 0;
}
