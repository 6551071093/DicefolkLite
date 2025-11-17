#pragma once
#include <string>
#include <vector>

enum class DiceFace {
    Attack,
    Shield,
    BuffATK,
    RotateLeft,
    RotateRight, 
    EmptyFace
};

// struct DiceInfo được định nghĩa ở đây (và chỉ ở đây)
struct DiceInfo {
    DiceFace type;
    int value;
    bool isUsed;
    std::string description;
};

class Dice {
    // Class rỗng, không cần khai báo hàm gì cả
};