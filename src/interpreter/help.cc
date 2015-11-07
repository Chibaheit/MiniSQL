#include <iostream>
#include <string>

#include "help.h"

namespace HELP {
    void help() {
        std::cout << std::endl;
        std::cout << "Usage: minisql [programfile] [arguments]" << std::endl;
        std::cout << "  --help     show this message" << std::endl;
        std::cout << "Features:" << std::endl;
        std::cout << "  dump       dump data from minisql" << std::endl;
        std::cout << "  import     import data to minisql" << std::endl;
    }
}
