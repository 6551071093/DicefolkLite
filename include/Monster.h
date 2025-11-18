#pragma once
#ifndef MONSTER_H
#define MONSTER_H

#include <string>
using namespace std;

class Monster {
private:
    string name;
    int maxHP;
    int currentHP;
    int baseAttack; // Công cơ bản
    int attackBuff; // Lượng công được buff
    int buffDuration; // Số lượt buff còn lại
    int shield;
    int itemSlots;
    string spritePath;

public:
    Monster(const string& n, int hp, int atk, const string& sprite);

    void takeDamage(int dmg);
    void healFull();
    void addShield(int val);
    void increaseMaxHP(int v);
    void addItemSlot();
    
    // === CÁC HÀM MỚI CHO BATTLE.CPP (V2) ===
    
    // Kích hoạt buff (Battle.cpp gọi)
    void buffAttack(int v); 
    
    // Giảm thời gian buff (Battle.cpp gọi)
    void endTurn();   
    
    // Reset Khiên (Battle.cpp gọi)
    void resetRoundStats(); 

    // === CÁC HÀM GETTER ===
    bool isAlive() const;
    string getName() const;
    int getHP() const { return currentHP; }
    int getMaxHP() const { return maxHP; }
    int getShield() const { return shield; }
    int getItemSlots() const { return itemSlots; }
    string getSpritePath() const { return spritePath; }
    
    // Trả về tổng ATK (Base + Buff) (Battle.cpp gọi)
    int getATK() const; 
    
    // Trả về thời gian buff (Battle.cpp gọi)
    int getBuffDuration() const { return buffDuration; }
};

#endif // MONSTER_H