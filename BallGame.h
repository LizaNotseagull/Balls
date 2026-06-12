#ifndef BALL_GAME_H
#define BALL_GAME_H

#include "DoublyLinkedList.h"
#include "Ball.h"
#include <string>

/**
 * Класс, управляющий игрой "Шарики".
 * Хранит последовательность шариков в двусвязном списке.
 * Реализует удаление цепочек и взрывы бомб.
 */
class BallGame {
private:
    DoublyLinkedList<Ball> balls;   // список шариков
    int totalDestroyed;              // количество уничтоженных за последний вызов runDestruction
    bool bombsEnabled;               // флаг разрешения бомб (true – продвинутый режим)

    // Вспомогательная структура для хранения границ цепочки
    struct ChainInfo;
    ChainInfo findFirstNormalChain() const;
    bool findAndExplodeBomb();
    void destroyChain(ChainInfo& chain);
    int getMinChainLength() const;   // всегда 3 (но оставлена для возможного расширения)

public:
    BallGame();
    ~BallGame() = default;

    void setBombsEnabled(bool enabled);
    bool isBombsEnabled() const { return bombsEnabled; }
    void clear();
    void addBall(const Ball& ball);
    void loadFromFile(const std::string& filename, int maxCount = 200000);
    void saveToFile(const std::string& filename) const;
    void generateRandom(int count, int numColors, int maxCount = 200000);
    void insertBallAt(int position, const Ball& ball);
    std::string getStateString() const;
    int runDestruction();            // запуск процесса удаления
    size_t getSize() const;

    // Проверка условия задачи: не более одной начальной цепочки
    int countInitialChains() const;
};

#endif