#include <iostream>

#include "Program.hpp"

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    // freopen("../testcases/43.in", "r", stdin);
    // freopen("../ans.out", "w", stdout);

    Program program;
    std::string line;
    while (program.programRun() && std::getline(std::cin, line)) {
        if (line.empty()) {
            continue;
        }
        program.execute(line);
    }
    return 0;
}