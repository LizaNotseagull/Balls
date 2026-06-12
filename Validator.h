#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <string>
#include "exceptions.h"

/**
 * Класс со статическими методами для валидации входных данных.
 * Все методы выбрасывают соответствующие исключения при нарушении.
 */
class Validator {
public:
    static void validateColor(int color);
    static void validateCount(int count, int maxCount = 200000);
    static void validatePosition(int position, size_t listSize);
    static void validateDataLoaded(bool dataLoaded);
};

#endif