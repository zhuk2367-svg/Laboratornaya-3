#include "../include/evaluator.h"
#include <cmath>
#include <iostream>

using namespace std;

void printResult(double res) {
    if (std::isinf(res)) {
        cout << (res > 0 ? "inf" : "-inf") << "\n";
    } else if (std::isnan(res)) {
        cout << "nan\n";
    } else {
        cout << res << "\n";
    }
}
