#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

/**
 * Базовый класс для всех исключений игры.
 * Используется для единой обработки всех ошибок через catch(GameException&).
 */
class GameException : public std::exception {
protected:
    std::string message;  // текст сообщения об ошибке
public:
    GameException(const std::string& msg);
    virtual const char* what() const noexcept override;
};

// Специфичные исключения (каждое имеет свой конструктор с нужными параметрами)
class EmptyListException : public GameException {
public:
    EmptyListException();
};

class InvalidColorException : public GameException {
public:
    InvalidColorException(int c);
};

class FileOpenException : public GameException {
public:
    FileOpenException(const std::string& filename);
};

class InvalidInputException : public GameException {
public:
    InvalidInputException(const std::string& details);
};

class TooManyBallsException : public GameException {
public:
    TooManyBallsException(int max);
};

class InvalidPositionException : public GameException {
public:
    InvalidPositionException();
};

#endif
