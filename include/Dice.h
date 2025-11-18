#pragma once
#ifndef DICE_H
#define DICE_H

#include <vector>
#include <string>
using namespace std;

enum class DiceFace {
    Attack,
    Shield,
    AttackOneOfThree, // <-- File gốc có cái này
    BuffATK,
    EmptyFace,       
    RotateLeft,
    RotateRight,
    RotateChoice,
    RotateRandom
};

enum class DiceType {
    A, // action
    B  // direction
};

// ==========================================================
// === BỔ SUNG STRUCT DICEINFO MÀ BATTLE.CPP (V2) CẦN ===
// (Nó không dùng DiceFace gốc mà dùng struct này)
// ==========================================================
struct DiceInfo {
    DiceFace type = DiceFace::Attack; // Tận dụng enum cũ
    int value = 0;
    string description = "";
    bool isUsed = false;
};

class Dice {
private:
    DiceType type;
    vector<DiceFace> faces;
    int lastIndex;

    void setupFaces(); // dùng khi khởi tạo
public:
    Dice(DiceType t = DiceType::A);
    DiceFace roll();           // random và lưu lastIndex
    DiceFace getResult() const;
    int getLastIndex() const;
    string faceToString(DiceFace f) const;
    DiceType getType() const;
};

#endif // DICE_H
