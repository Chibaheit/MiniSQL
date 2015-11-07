#ifndef MINISQL_REPL_H
#define MINISQL_REPL_H

#include <string>

namespace REPL {
    void makeInstruction(std::string instruction);
    void repl(std::string VERSION);
    void file(std::string fileName);
}

#endif
