#include "../include/utils.h"

string formatDouble(double v) {
    string s = to_string(v);
    s.erase(s.find_last_not_of('0') + 1, string::npos);
    if (!s.empty() && s.back() == '.') s.pop_back();
    return s;
}