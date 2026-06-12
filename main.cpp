#include "BallGame.h"
#include "Validator.h"
#include "utils.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

void dataMenu(BallGame& game, bool& dataLoaded);
void insertionGame(BallGame& game, bool& dataLoaded);

int main() {
    std::setlocale(LC_ALL, "Russian");
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    BallGame game;
    bool dataLoaded = false;
    int mainChoice;

    do {
        std::cout << "\n========== ШАРИКИ ==========\n";
        std::cout << "1. Новая игра\n2. Продолжить текущую игру\n3. Игра с вставкой\n4. Выход\nВыберите пункт: ";
        mainChoice = safeInputInt("");

        if (mainChoice == 1) {
            std::cout << "\nВыберите режим:\n1. Классический (без бомба-шаров)\n2. Продвинутый (с бомба-шарами)\nВаш выбор: ";
            int mode = safeInputInt("");
            if (mode == 1) game.setBombsEnabled(false);
            else if (mode == 2) game.setBombsEnabled(true);
            else { std::cout << "Неверный режим, возврат.\n"; continue; }
            dataLoaded = false;
            dataMenu(game, dataLoaded);
        }
        else if (mainChoice == 2) {
            try {
                Validator::validateDataLoaded(dataLoaded);
                game.runDestruction();
                std::cout << "Итоговое состояние: " << game.getStateString() << std::endl;
                if (game.getSize() == 0) {
                    std::cout << "\n=^..^= Шариков больше нет! Начните новую игру.\n";
                    dataLoaded = false;
                    game.clear();
                }
            }
            catch (const GameException& e) {
                std::cout << e.what() << std::endl;
            }
        }
        else if (mainChoice == 3) {
            insertionGame(game, dataLoaded);
        }
        else if (mainChoice == 4) {
            std::cout << "До свидания!\n";
        }
        else {
            std::cout << "Неверный пункт.\n";
        }
    } while (mainChoice != 4);

    return 0;
}