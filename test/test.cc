#include <iostream>
#include <cstdio>
#include <cassert>

#include "../src/interpreter/command.h"
#include "../src/interpreter/input.h"
#include "../src/interpreter/param.h"
#include "../src/api/api.h"
#include "../src/buffer/buffer.h"
using namespace std;

const string VERSION = "0.0.1";
int defaultNumBlocks = 2, defaultBlockSize = 4096;
void gao(string instruction) {
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

int main(int argc, const char* argv[]) {
    auto args = COMMAND::analyse(argc, argv);
    // it means ./minisql
    if (argc == 1) {
        //initializationCLI();
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
                gao(instruction);
                cout << endl;
            }
            INPUT::saveCommand();
        }
    }

    if (argc == 2) {
        if (args.find("help") != args.end()) {
            cout << "help" << endl;
        } else if (args.find("file") != args.end()) {
            //initializationCLI();
            FILE *fp;
            if ((fp = fopen(args["file"].c_str(), "r")) == NULL) {
                assert(fp = fopen(args["file"].c_str(), "r"));
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
                    gao(instruction);
                    cout << endl;
                }
            }
            fclose(fp);
        } else {
            cout << "Error: Invalid argument." << endl;
        }
    }
    Buffer::flush();
    return 0;
}
