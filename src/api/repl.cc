#include <iostream>
#include <string>

#include "repl.h"
#include "../interpreter/input.h"
#include "../interpreter/param.h"
#include "../api/api.h"
#include "../buffer/buffer.h"

namespace REPL {
    void makeInstruction(string instruction) {
        cout << instruction << endl;
        instruction = PARAM::rmSpaceAndBracket(instruction);
        cout << instruction << endl;
        try {
            if (instruction.find("create table ") == 0) {
                API::createTable(instruction);
            } else if (instruction.find("drop table ") == 0) {
                API::dropTable(instruction);
            } else if (instruction.find("create index ") == 0) {
                API::createIndex(instruction);
            } else if (instruction.find("drop index ") == 0) {
                API::dropIndex(instruction);
            } else if (instruction.find("select * from ") == 0) {
                API::select(instruction);
            } else if (instruction.find("insert into ") == 0) {
                API::insert(instruction);
            } else if (instruction.find("delete from ") == 0) {
                API::deleteFrom(instruction);
            } else if (instruction == "quit") {
                API::quit(instruction);
            } else if (instruction.find("execfile ") == 0) {
                API::execfile(instruction);
            } else {
                throw invalid_argument("Unknown sql script.");
            }
        } catch (invalid_argument& e) {
            cout << "Syntax Error: " << e.what() << endl;
        }
    }
    void repl(string VERSION) {
        string chunk = "", buffer = "", instruction = "";
        while (INPUT::readLine(chunk, buffer.length() == 0 ? (VERSION + "> ") : (VERSION + "? "))) {
            if (buffer.length() == 0) {
                buffer = chunk;
            } else {
                buffer = buffer + " " + chunk;
            }
            size_t semiPosition;
            while ((semiPosition = buffer.find(";")) != string::npos) {
                instruction = buffer.substr(0, semiPosition);
                buffer = buffer.substr(semiPosition + 1);
                makeInstruction(instruction);
                cout << endl;
            }
            INPUT::saveCommand();
        }
        Buffer::flush();
    }
    void file(string fileName) {
        FILE *fp;
        if ((fp = fopen(fileName.c_str(), "r")) == NULL) {
            assert(fp = fopen(fileName.c_str(), "r"));
        }
        string buffer = "", instruction = "";
        char line[1024];
        while (!feof(fp)) {
            fgets(line, 1024, fp);
            line[strlen(line) - 1] = '\0';
            if (buffer.length() != 0) {
                buffer = buffer + " " + string(line);
            } else {
                buffer = string(line);
            }
            size_t semiPosition;
            while ((semiPosition = buffer.find(";")) != string::npos) {
                instruction = buffer.substr(0, semiPosition);
                buffer = buffer.substr(semiPosition + 1);
                makeInstruction(instruction);
                cout << endl;
            }
        }
        fclose(fp);
        Buffer::flush();
    }
}
