#include "BallGame.h"
#include "Validator.h"
#include "utils.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>

static void inputBallsClassic(BallGame& game) {
    int n = safeInputInt("Введите количество шариков: ");
    Validator::validateCount(n);
    std::cout << "Введите " << n << " цветов (0-9) через пробел: ";
    for (int i = 0; i < n; ++i) {
        int color;
        std::cin >> color;
        if (std::cin.fail()) {
            clearInputStream();
            throw InvalidInputException("Ошибка ввода цвета");
        }
        Validator::validateColor(color);
        game.addBall(Ball(BallType::NORMAL, color));
    }
}

static void inputBallsWithBombs(BallGame& game) {
    int n = safeInputInt("Введите количество шариков: ");
    Validator::validateCount(n);
    std::cout << "Введите " << n << " элементов (0-9 - обычный, B - бомба-шар) через пробел: ";
    for (int i = 0; i < n; ++i) {
        std::string token;
        std::cin >> token;
        if (std::cin.fail()) {
            clearInputStream();
            throw InvalidInputException("Ошибка ввода");
        }
        if (token == "B" || token == "b") {
            game.addBall(Ball(BallType::BOMB, -1));
        }
        else {
            // Пытаемся преобразовать в число с обработкой исключений
            int color;
            try {
                color = std::stoi(token);
            }
            catch (const std::invalid_argument&) {
                throw InvalidInputException("Некорректный ввод: '" + token + "'. Ожидалась цифра от 0 до 9 или буква B.");
            }
            catch (const std::out_of_range&) {
                throw InvalidInputException("Число вне допустимого диапазона: " + token);
            }
            Validator::validateColor(color);
            game.addBall(Ball(BallType::NORMAL, color));
        }
    }
}

void dataMenu(BallGame& game, bool& dataLoaded) {
    int choice;
    do {
        std::cout << "\n--- ИСТОЧНИК ДАННЫХ ---\n";
        std::cout << "1. Ввести с клавиатуры (обычные шарики)\n";
        if (game.isBombsEnabled()) {
            std::cout << "2. Ввести с клавиатуры (с бомба-шарами)\n";
        }
        else {
            std::cout << "2. (недоступно в классическом режиме)\n";
        }
        std::cout << "3. Загрузить из файла\n";
        std::cout << "4. Случайная генерация\n";
        std::cout << "5. Показать текущее состояние\n";
        std::cout << "6. Назад в главное меню\n";
        std::cout << "Выберите пункт: ";
        std::cin >> choice;
        if (std::cin.fail()) {
            clearInputStream();
            std::cout << "Ошибка ввода\n";
            continue;
        }
        try {
            bool loaded = false;
            switch (choice) {
            case 1:
                game.clear();
                inputBallsClassic(game);
                loaded = true;
                break;
            case 2:
                if (!game.isBombsEnabled()) {
                    std::cout << "Классический режим: бомбы запрещены.\n";
                    break;
                }
                game.clear();
                inputBallsWithBombs(game);
                loaded = true;
                break;
            case 3: {
                std::string fname;
                std::cout << "Имя файла: ";
                std::cin >> fname;
                game.loadFromFile(fname);
                loaded = true;
                break;
            }
            case 4: {
                int count = safeInputInt("Количество шариков: ");
                int colors = safeInputInt("Количество различных цветов (1-10): ");
                if (colors < 1 || colors > 10) colors = 5;
                game.generateRandom(count, colors);
                loaded = true;
                break;
            }
            case 5:
                std::cout << "Текущее состояние: " << game.getStateString() << "\n";
                break;
            case 6:
                return;
            default:
                std::cout << "Неверный пункт.\n";
            }
            if (loaded) {
                dataLoaded = true;
                // *** ПРОВЕРКА УСЛОВИЯ ЗАДАЧИ ***
                int chains = game.countInitialChains();
                if (chains > 1) {
                    std::cout << "\nОшибка: в начальных данных обнаружено " << chains
                        << " цепочек из 3+ одинаковых шариков.\n"
                        << "По условию задачи допускается не более одной. Данные не будут обработаны.\n";
                    dataLoaded = false;
                    game.clear();
                    return;
                }
                std::cout << "\nИсходное состояние: " << game.getStateString() << std::endl;
                std::cout << "Начинаем удаление цепочек...\n";
                game.runDestruction();   // вывод итога внутри
                return;
            }
        }
        catch (const GameException& e) {
            std::cerr << "Ошибка: " << e.what() << std::endl;
            dataLoaded = false;
            game.clear();
            clearInputStream();
        }
    } while (true);
}

void insertionGame(BallGame& game, bool& dataLoaded) {
    try {
        Validator::validateDataLoaded(dataLoaded);
    }
    catch (const GameException& e) {
        std::cout << e.what() << std::endl;
        return;
    }
    std::cout << "\n=== ИГРА С ВСТАВКОЙ ===\n";
    std::cout << "Текущее состояние: " << game.getStateString() << "\n";
    while (true) {
        if (game.getSize() == 0) {
            std::cout << "\n=^..^= Шариков больше нет! Начните новую игру.\n";
            dataLoaded = false;
            game.clear();
            return;
        }
        std::cout << "\n1. Вставить шарик\n2. Запустить удаление цепочек\n3. Показать состояние\n4. Вернуться в главное меню\nВыбор: ";
        int ch = safeInputInt("");
        if (ch == 4) break;
        try {
            if (ch == 1) {
                std::cout << "Введите тип (0-обычный, 1-бомба-шар): ";
                int t;
                std::cin >> t;
                if (t == 1 && !game.isBombsEnabled()) {
                    std::cout << "Классический режим: бомба-шары запрещены.\n";
                    continue;
                }
                BallType type = (t == 1) ? BallType::BOMB : BallType::NORMAL;
                int color = 0;
                if (type == BallType::NORMAL) {
                    color = safeInputInt("Цвет (0-9): ");
                    Validator::validateColor(color);
                }
                else {
                    color = -1;
                }
                int pos = safeInputInt("Позиция (0-" + std::to_string(game.getSize()) + "): ");
                Validator::validatePosition(pos, game.getSize());
                game.insertBallAt(pos, Ball(type, color));
                std::cout << "Вставлено. Новое состояние: " << game.getStateString() << "\n";
            }
            else if (ch == 2) {
                game.runDestruction();
                std::cout << "Состояние: " << game.getStateString() << "\n";
                if (game.getSize() == 0) {
                    std::cout << "\n=^..^= Шариков больше нет! Начните новую игру.\n";
                    dataLoaded = false;
                    game.clear();
                    return;
                }
            }
            else if (ch == 3) {
                std::cout << "Состояние: " << game.getStateString() << "\n";
            }
        }
        catch (const GameException& e) {
            std::cerr << "Ошибка: " << e.what() << std::endl;
        }
    }
}
