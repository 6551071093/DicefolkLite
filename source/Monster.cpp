#include "Monster.h"
#include <algorithm>

using namespace std;

// Khởi tạo baseAttack, buff = 0
Monster::Monster(const string& n, int hp, int atk, const string& sprite)
    : name(n), maxHP(hp), currentHP(hp), 
      baseAttack(atk), attackBuff(0), buffDuration(0), // Logic Buff
      shield(0), itemSlots(0), spritePath(sprite)
{}

void Monster::takeDamage(int dmg) {
    if (dmg <= 0) return;

    // Trừ vào khiên trước
    if (shield > 0) {
        int absorb = std::min(shield, dmg);
        shield -= absorb;
        dmg -= absorb;
    }
    // Trừ vào máu
    if (dmg > 0) {
        currentHP -= dmg;
        if (currentHP < 0) currentHP = 0;
    }
}

// === TRIỂN KHAI LOGIC BUFF (V2) ===

void Monster::buffAttack(int v) {
    // Chỉ nhận giá trị buff, không cộng dồn
    attackBuff = v; 
    buffDuration = 2; // Buff tồn tại 2 lượt
}

void Monster::endTurn() {
    // Giảm thời gian buff mỗi cuối lượt
    if (buffDuration > 0) {
        buffDuration--;
        if (buffDuration == 0) {
            attackBuff = 0; // Hết buff, reset
        }
    }
}

void Monster::resetRoundStats() {
    // Battle.cpp (V2) gọi hàm này đầu mỗi Round
    // Tác dụng: Reset khiên về 0
    shield = 0;
}

// Hàm getATK trả về tổng (Base + Buff)
int Monster::getATK() const {
    if (buffDuration > 0) {
        return baseAttack + attackBuff;
    }
    return baseAttack; // Chỉ trả về base nếu hết buff
}


// === CÁC HÀM CŨ (Giữ nguyên) ===
void Monster::addShield(int val) { if (val > 0) shield += val; }
void Monster::increaseMaxHP(int v) { maxHP += v; currentHP += v; }
void Monster::healFull() { currentHP = maxHP; }
void Monster::addItemSlot() { itemSlots++; }
bool Monster::isAlive() const { return currentHP > 0; }
string Monster::getName() const { return name; }
// === LỖI REDEFINITION ĐÃ ĐƯỢC SỬA (XÓA HÀM getSpritePath() KHỎI ĐÂY) ===
// Các hàm getHP, getMaxHP, getShield, getItemSlots, getSpritePath 
// đã được định nghĩa inline trong Monster.h