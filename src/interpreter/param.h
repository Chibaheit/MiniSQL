#ifndef PARAM_H
#define PARAM_H

#include <string>
#include <vector>

namespace PARAM {
    int Int(std::string s);
    float Float(std::string s);
    std::string Name(std::string s);
    std::string rmSpaceAndBracket(std::string s);
    std::vector<std::string> Split(std::string& s, char separator = ' ');
}

#endif
