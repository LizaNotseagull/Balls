#include "Validator.h"

void Validator::validateColor(int color) {
    if (color < 0 || color > 9) throw InvalidColorException(color);
}

void Validator::validateCount(int count, int maxCount) {
    if (count < 0) throw InvalidInputException("Количество шариков не может быть отрицательным");
    if (count > maxCount) throw TooManyBallsException(maxCount);
}

void Validator::validatePosition(int position, size_t listSize) {
    if (position < 0 || position > static_cast<int>(listSize)) throw InvalidPositionException();
}

void Validator::validateDataLoaded(bool dataLoaded) {
    if (!dataLoaded) throw InvalidInputException("Данные не загружены. Сначала создайте новую игру.");
}
