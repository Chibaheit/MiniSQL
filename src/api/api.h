#ifndef API_H
#define API_H

#include <string>

namespace API {
    void createTable(std::string instruction);

    void dropTable(std::string instruction);

    void createIndex(std::string instruction);

    void dropIndex(std::string instruction);

    void select(std::string instruction);

    void insert(std::string instruction);

    void deleteFrom(std::string instruction);

    void quit(std::string instruction);

    void execfile(std::string instruction);

}

#endif
