#include "utils.h"
#include <iostream>
#include <limits>

void clearInputStream() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int safeInputInt(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail()) {
            clearInputStream();
            std::cout << "Ошибка: введите целое число.\n";
        }
        else {
            clearInputStream();
            return value;
        }
    }
}
