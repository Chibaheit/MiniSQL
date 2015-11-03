#include "input.h"

#include <cstdlib>
#include <readline/readline.h>
#include <readline/history.h>
// use history to save minisql script

using namespace std;

bool INPUT::readLine(std::string& chunk, std::string shell_prompt) {
    char *input;

    // Display prompt and read input (input must be freed after use)...
    input = readline(shell_prompt.c_str());

    // Check for EOF.
    if (!input) {
        return false;
    }

    // Add input to history.
    add_history(input);

    // Do stuff...
    chunk = std::string(input);

    // Free input.
    free(input);

    return true;
}

int INPUT::initialization() {
    return read_history(NULL);
}


int INPUT::saveCommand() {
    return write_history(NULL);
}
