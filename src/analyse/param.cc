#include <stdexcept>
#include <cctype>
#include <cmath>

#include "param.h"

using namespace std;

namespace PARAM {
    int Int(string s) {
        int res = 0;
        size_t i = 0;
        int sign = 1;
        if (s[0] == '-') {
            sign = -1;
            i = 1;
            if (s.length() == 1) {
                throw invalid_argument(s + " isn't a valid integer number.");
            }
        }
        for (; i < s.length(); i++) {
            if (isdigit(s[i])) {
                res = res * 10 + s[i] - '0';
            } else {
                throw invalid_argument(s + " isn't a valid integer number.");
            }
        }
        return (res * sign);
    }
    float Float(string s) {
        float res = 0;
        size_t i = 0;
        int sign = 1;
        if (s[0] == '-') {
            sign = -1;
            i = 1;
            if ((s.length() == 1) || (s.length() == 2 && s[1] == '.')) {
                throw invalid_argument(s + " isn't a valid flaot number.");
            }
        }
        int decimalPos = 0;
        for (; i < s.length(); ++i) {
            if (isdigit(s[i])) {
                if (decimalPos == 0) {
                    res = res * 10 + s[i] - '0';
                } else {
                    res = res + (s[i] - '0') / (pow(10, decimalPos));
                    ++decimalPos;
                }
            } else {
                if (decimalPos == 0 && s[i] == '.') {
                    ++decimalPos;
                } else {
                    throw invalid_argument(s + " isn't a valid float number.");
                }
            }
        }
        return (res * sign);
    }
    string Name(string s) {
        if (s.length() == 0 || !isalpha(s[0])) {
            throw invalid_argument(s + " isn't a valid name.");
        }
        for (size_t i = 0; i < s.length(); i++) {
            if (s[i] != '_' && !isalnum(s[i])) {
                throw invalid_argument(s + " isn't a valid name.");
            }
        }
        return s;
    }
}
