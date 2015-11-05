#include <stdexcept>
#include <cctype>
#include <cmath>
#include <iostream>

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

    string rmSpaceAndBracket(string s) {
        string res = "";
        string rS = string(s.rbegin(), s.rend());
        size_t pos, rPos;
        size_t length = s.length();
        try {
            pos = s.find('('); rPos = rS.find(')');
            s[pos] = ' '; s[length - 1 - rPos] = ' ';

            if ((pos != string::npos && rPos == string::npos) || (pos == string::npos && rPos != string::npos) || (pos + rPos > length - 1)) {
                throw invalid_argument("unmatched bracket.");
            }
        } catch (invalid_argument& e) {
            cout << "Syntax Error: " << e.what() << endl;
        }
        size_t st = 0, ed = s.length();
        while (ed > 0 && s[ed - 1] == ' ') {
          --ed;
        }
        while (st < ed && s[st] == ' ') {
          ++st;
        }
        bool space = true;
        for (size_t i = st; i < ed; i++) {
            if (space && s[i] == ' ') {
                continue;
            }
            if (s[i] == ')' && res.length() > 1 && res[res.length() - 1] == ' ') {
                res = res.substr(0, res.length() - 1);
            }
            res += s[i];
            space = (s[i] == ' ');
            if (s[i] == '(') {
              if (res.length() > 2 && res[res.length() - 2] == ' ') {
                  res.erase(res.begin() + res.length() - 2);
              }
              if (i + 1 < ed && s[i + 1] == ' ') {
                  space = true;
                  continue;
              }
            }
        }
        return res;
    }

    vector<string> Split(string& s, char separator) {
        vector<string> res;
        string tmp = "";
        for (size_t i = 0; i < s.length(); i++) {
            if (s[i] == separator) {
                res.push_back(tmp);
                tmp = "";
            } else {
                tmp += s[i];
            }
        }
        if (tmp != "") {
            res.push_back(tmp);
        }
        return res;
    }
}
