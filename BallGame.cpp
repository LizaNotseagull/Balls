#include "BallGame.h"
#include "exceptions.h"
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <iostream>

struct BallGame::ChainInfo {
    Node<Ball>* start;
    Node<Ball>* end;
    int length;
    ChainInfo() : start(nullptr), end(nullptr), length(0) {}
};

BallGame::BallGame() : totalDestroyed(0), bombsEnabled(false) {}

void BallGame::setBombsEnabled(bool enabled) { bombsEnabled = enabled; }
void BallGame::clear() { balls.clear(); totalDestroyed = 0; }

void BallGame::addBall(const Ball& ball) {
    if (!bombsEnabled && ball.type == BallType::BOMB)
        throw InvalidColorException(100);
    balls.push_back(ball);
}
size_t BallGame::getSize() const { return balls.size(); }
int BallGame::getMinChainLength() const { return 3; }

/**
 * Поиск первой непрерывной цепочки обычных шариков одного цвета длиной >= 3.
 * Возвращает структуру ChainInfo с указателями на начало и конец цепочки.
 * Если цепочка не найдена, длина = 0.
 */
BallGame::ChainInfo BallGame::findFirstNormalChain() const {
    ChainInfo info;
    if (balls.empty()) return info;
    Node<Ball>* curr = balls.get_head();
    while (curr) {
        if (curr->getData().type == BallType::NORMAL) {
            int color = curr->getData().color;
            int len = 1;
            Node<Ball>* nextNode = curr->getNext();
            // Идём вперёд, пока встречаются такие же обычные шарики
            while (nextNode && nextNode->getData().type == BallType::NORMAL && nextNode->getData().color == color) {
                ++len;
                nextNode = nextNode->getNext();
            }
            if (len >= 3) {
                info.start = curr;
                info.end = nextNode ? nextNode->getPrev() : balls.get_tail();
                info.length = len;
                return info;
            }
            curr = nextNode;
        }
        else {
            curr = curr->getNext();
        }
    }
    return info;
}

/**
 * Поиск и взрыв первой встреченной бомбы.
 * Удаляет саму бомбу и до двух соседей слева и справа (если они есть).
 * Возвращает true, если бомба была найдена и взорвана.
 * Сложность: O(n) в худшем случае, но в среднем быстро.
 */
bool BallGame::findAndExplodeBomb() {
    if (!bombsEnabled) return false;
    Node<Ball>* curr = balls.get_head();
    while (curr) {
        if (curr->getData().type == BallType::BOMB) {
            // Массив для хранения указателей на удаляемые узлы (максимум 5)
            Node<Ball>* toDelete[5] = { nullptr };
            int cnt = 0;
            // Сосед через один слева
            Node<Ball>* left2 = curr->getPrev() ? curr->getPrev()->getPrev() : nullptr;
            if (left2) toDelete[cnt++] = left2;
            // Непосредственный левый сосед
            Node<Ball>* left1 = curr->getPrev();
            if (left1) toDelete[cnt++] = left1;
            // Сама бомба
            toDelete[cnt++] = curr;
            // Непосредственный правый сосед
            Node<Ball>* right1 = curr->getNext();
            if (right1) toDelete[cnt++] = right1;
            // Сосед через один справа
            Node<Ball>* right2 = curr->getNext() ? curr->getNext()->getNext() : nullptr;
            if (right2) toDelete[cnt++] = right2;

            // Удаляем все собранные узлы
            for (int i = 0; i < cnt; ++i) {
                if (toDelete[i]) {
                    balls.remove(toDelete[i]);
                    totalDestroyed++;
                }
            }
            return true;
        }
        curr = curr->getNext();
    }
    return false;
}

void BallGame::destroyChain(ChainInfo& chain) {
    if (!chain.start || !chain.end || chain.length < 3) return;
    balls.remove_range(chain.start, chain.end);
    totalDestroyed += chain.length;
}

void BallGame::loadFromFile(const std::string& filename, int maxCount) {
    std::ifstream file(filename);
    if (!file.is_open()) throw FileOpenException(filename);
    clear();
    int n;
    file >> n;
    if (file.fail()) throw InvalidInputException("Не удалось прочитать количество шариков");
    if (n < 0 || n > maxCount) throw TooManyBallsException(maxCount);

    for (int i = 0; i < n; ++i) {
        int typeInt, color;
        file >> typeInt >> color;
        if (file.fail()) throw InvalidInputException("Недостаточно данных в файле");
        BallType type = static_cast<BallType>(typeInt);
        if (!bombsEnabled && type == BallType::BOMB)
            throw InvalidInputException("В классическом режиме нельзя загружать бомбы");
        if (type == BallType::NORMAL && (color < 0 || color > 9)) throw InvalidColorException(color);
        balls.push_back(Ball(type, color));
    }
    file.close();
}

void BallGame::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) throw FileOpenException(filename);
    file << balls.size() << "\n";
    for (auto it = balls.cbegin(); it != balls.cend(); ++it) {
        int typeInt = static_cast<int>((*it).type);
        int color = (*it).color;
        file << typeInt << " " << color << "\n";
    }
    file.close();
}

void BallGame::generateRandom(int count, int numColors, int maxCount) {
    if (count > maxCount) throw TooManyBallsException(maxCount);
    clear();
    for (int i = 0; i < count; ++i) {
        if (bombsEnabled) {
            int r = rand() % 100;
            if (r < 8) {   // 8% вероятность бомбы
                balls.push_back(Ball(BallType::BOMB, -1));
                continue;
            }
        }
        int color = rand() % numColors;
        balls.push_back(Ball(BallType::NORMAL, color));
    }
}

void BallGame::insertBallAt(int position, const Ball& ball) {
    if (position < 0 || position > static_cast<int>(balls.size())) throw InvalidPositionException();
    if (!bombsEnabled && ball.type == BallType::BOMB)
        throw InvalidColorException(100);
    if (position == 0) {
        balls.push_front(ball);
    }
    else if (position == static_cast<int>(balls.size())) {
        balls.push_back(ball);
    }
    else {
        Node<Ball>* curr = balls.get_head();
        for (int i = 0; i < position; ++i) curr = curr->getNext();
        balls.insert_before(curr, ball);
    }
}

std::string BallGame::getStateString() const {
    if (balls.empty()) return "=^..^= (пусто)";
    std::string result;
    for (auto it = balls.cbegin(); it != balls.cend(); ++it) {
        if (!result.empty()) result += " ";
        if ((*it).type == BallType::NORMAL) {
            result += std::to_string((*it).color);
        }
        else {
            result += "B";
        }
    }
    return result;
}

/**
 * Подсчёт количества цепочек из трёх и более одинаковых обычных шариков.
 * Используется для проверки условия задачи (не более одной в начальном состоянии).
 * Возвращает количество найденных цепочек.
 */
int BallGame::countInitialChains() const {
    if (balls.empty()) return 0;
    int chains = 0;
    Node<Ball>* curr = balls.get_head();
    while (curr) {
        if (curr->getData().type == BallType::NORMAL) {
            int color = curr->getData().color;
            int len = 1;
            Node<Ball>* nextNode = curr->getNext();
            while (nextNode && nextNode->getData().type == BallType::NORMAL && nextNode->getData().color == color) {
                ++len;
                nextNode = nextNode->getNext();
            }
            if (len >= 3) {
                ++chains;
                if (chains > 1) return chains;  // оптимизация: больше одной – можно сразу вернуть
            }
            curr = nextNode;
        }
        else {
            curr = curr->getNext();
        }
    }
    return chains;
}

/**
 * Главный игровой цикл: удаляет цепочки и взрывает бомбы до полной стабилизации.
 * Выводит каждый шаг в консоль.
 * Возвращает общее количество уничтоженных шариков.
 */
int BallGame::runDestruction() {
    totalDestroyed = 0;
    bool changed = true;
    int step = 0;
    while (changed) {
        changed = false;
        // 1. Взрываем все обнаруженные бомбы (могут появляться новые после взрыва)
        while (findAndExplodeBomb()) {
            std::cout << "Шаг " << ++step << ": Взрыв бомбы! Состояние: " << getStateString() << std::endl;
            changed = true;
        }
        // 2. Удаляем все цепочки обычных шариков
        while (true) {
            ChainInfo chain = findFirstNormalChain();
            if (chain.length < 3) break;
            destroyChain(chain);
            std::cout << "Шаг " << ++step << ": Удалена цепочка из " << chain.length << " шариков. Состояние: " << getStateString() << std::endl;
            changed = true;
        }
    }
    std::cout << "\n=== Уничтожено шариков: " << totalDestroyed << " ===" << std::endl;
    return totalDestroyed;
}