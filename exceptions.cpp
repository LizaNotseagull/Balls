#include "exceptions.h"

GameException::GameException(const std::string& msg) : message(msg) {}
const char* GameException::what() const noexcept { return message.c_str(); }

EmptyListException::EmptyListException() : GameException("Ошибка: список пуст") {}
InvalidColorException::InvalidColorException(int c) : GameException("Недопустимое значение: " + std::to_string(c)) {}
FileOpenException::FileOpenException(const std::string& filename) : GameException("Не удалось открыть файл: " + filename) {}
InvalidInputException::InvalidInputException(const std::string& details) : GameException("Неверный формат данных: " + details) {}
TooManyBallsException::TooManyBallsException(int max) : GameException("Превышено максимальное количество шариков (макс " + std::to_string(max) + ")") {}
InvalidPositionException::InvalidPositionException() : GameException("Недопустимая позиция для вставки") {}