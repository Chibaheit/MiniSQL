#include "input.h"

#include <cstdlib>
#include <readline/readline.h>
#include <readline/history.h>
// use history to save minisql script

namespace INPUT {
    bool readLine(std::string& chunk, std::string shell_prompt) {
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

    int initialization() {
        return read_history(NULL);
    }


    int saveCommand() {
        return write_history(NULL);
    }
}
