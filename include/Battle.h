#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Monster.h"
#include "Dice.h" // <-- Đã bao gồm DiceInfo từ file này
#include "BattleConfig.h"

enum class BattleState {
    START_ROUND,
    WAIT_INPUT,      // Chờ người chơi
    ENEMY_THINKING,  // Chờ địch ra chiêu
    EXECUTING,       // Animation
    CHECK_END,
    FINISHED
};

// --- ĐÃ XÓA struct DiceInfo BỊ TRÙNG LẶP TẠI ĐÂY ---

class Battle {
public:
    Battle();
    void loadStage(int idx);
    void loadBoss();
    void setAllies(const std::vector<Monster>& a);

    void startRound(); 
    void update(float dt);
    void render(sf::RenderWindow& win);

    // Người chơi chọn xúc xắc số 1 hoặc 2 (index 0 hoặc 1)
    void playerSelectDie(int index);

    bool isFinished() const;
    bool playerWon() const;
    BattleState getState() const;

private:
    void executeDieEffect(DiceInfo& die, bool isPlayerAction);
    void rotateTeam(bool isPlayerTeam, bool left);
    void calculateFormationPositions(bool isPlayerTeam);
    
    // Vẽ 2 hàng xúc xắc (Ta và Địch)
    void renderDiceGroups(sf::RenderWindow& win);

    struct AnimatedMonster {
        Monster monster;
        sf::Vector2f currentPos;
        sf::Vector2f targetPos;
        float moveTime;
        float moveDuration;
    };
    std::vector<AnimatedMonster> animatedAllies;
    std::vector<AnimatedMonster> animatedEnemies;

    std::vector<Monster> allies;
    std::vector<Monster> enemies;
    
    // --- CƠ CHẾ MỚI: 2 KHO XÚC XẮC RIÊNG BIỆT ---
    std::vector<DiceInfo> playerDice; // 2 xúc xắc của người chơi
    std::vector<DiceInfo> enemyDice;  // 2 xúc xắc của địch (copy từ người chơi)
    
    // --- THỨ TỰ KHAI BÁO (QUAN TRỌNG CHO -Wreorder) ---
    bool isAllyTurn;                 // 1.
    float actionTimer;               // 2.
    BattleState state;               // 3.
    bool winFlag;                    // 4.
    // --------------------------------------------

    sf::Font font;
    sf::Text infoText;
    sf::Texture backgroundTex;
    sf::Sprite backgroundSprite;
};