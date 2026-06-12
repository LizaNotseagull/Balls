#ifndef BALL_H
#define BALL_H

enum class BallType {
    NORMAL,   // обычный шарик (цвет 0-9)
    BOMB      // бомба (при взрыве уничтожает соседей)
};

struct Ball {
    BallType type;
    int color;   // для NORMAL: 0-9, для BOMB: -1 (не используется)

    Ball(BallType t = BallType::NORMAL, int c = 0);
    // Оператор сравнения нужен для поиска цепочек (сравниваются только обычные шарики по цвету)
    bool operator==(const Ball& other) const {
        if (type != other.type) return false;
        if (type == BallType::NORMAL) return color == other.color;
        return false;
    }
};

#endif
