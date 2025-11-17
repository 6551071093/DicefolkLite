// === SỬA LỖI M_PI: Phải định nghĩa TRƯỚC khi include cmath ===
#define _USE_MATH_DEFINES 

#include "Battle.h"
#include "Utils.h"
#include <iostream>
#include <algorithm>
#include <map>
#include <cmath> // Bây giờ sẽ bao gồm M_PI

using namespace std;

static int findFirstAlive(const vector<Monster>& team) {
    for (int i = 0; i < (int)team.size(); ++i) if (team[i].isAlive()) return i;
    return -1;
}

static std::map<string, sf::Texture> textureCache;

// === SỬA LỖI -Wreorder: Sắp xếp lại thứ tự khởi tạo khớp với Battle.h ===
Battle::Battle()
    : isAllyTurn(true),                // 1.
      actionTimer(0.f),              // 2.
      state(BattleState::START_ROUND), // 3.
      winFlag(false)                   // 4.
{
    if (!font.loadFromFile("../assets/fonts/arial.ttf")) cerr << "Err font\n";
    if (backgroundTex.loadFromFile("../assets/sprites/Background_Battle.png")) {
        backgroundSprite.setTexture(backgroundTex);
        // Thiết lập tỉ lệ cho Background: màn hình 1280x720, ảnh 1248x832
        sf::Vector2u texSize = backgroundTex.getSize();
        backgroundSprite.setScale(1280.f / texSize.x, 720.f / texSize.y);
    }

    infoText.setFont(font);
    infoText.setCharacterSize(24);
    infoText.setFillColor(sf::Color::White);
    infoText.setOutlineColor(sf::Color::Black);
    infoText.setOutlineThickness(2.f);
    infoText.setPosition(350.f, 30.f);
}

void Battle::loadStage(int idx) {
    if (idx >= 0 && idx < (int)BattleConfig::stages.size()) enemies = BattleConfig::stages[idx].enemies;
    else enemies.clear();
}
void Battle::loadBoss() { enemies = BattleConfig::boss.enemies; }
// === SỬA LỖI CÚ PHÁP: Xóa thừa 'void' (Dòng 45) ===
void Battle::setAllies(const vector<Monster>& a) { 
    allies = a; 
    animatedAllies.clear();
    // Tạo AnimatedMonster từ Monster
    for(const auto& m : allies) {
        animatedAllies.push_back({m, {0.f, 0.f}, {0.f, 0.f}, 0.f, 0.f}); 
    }
    calculateFormationPositions(true);
}

// =========================================================================
// === ĐIỀU CHỈNH VỊ TRÍ ĐỘI HÌNH THEO HÌNH ẢNH CỦA NGƯỜI DÙNG (Grid-based) ===
// =========================================================================
void Battle::calculateFormationPositions(bool isPlayerTeam) {
    vector<AnimatedMonster>& team = isPlayerTeam ? animatedAllies : animatedEnemies;
    
    // Vị trí cơ bản (BASE POS) cho quái vật đầu tiên (vị trí 0)
    // ĐỒNG MINH (TRÁI): X: 300.f, Y: 400.f
    // KẺ THÙ (PHẢI): X: 980.f, Y: 400.f
    const sf::Vector2f basePos = isPlayerTeam ? sf::Vector2f(300.f, 400.f) : sf::Vector2f(980.f, 400.f);
    
    // Khoảng cách giữa các quái vật theo X
    // Đồng minh: Tăng X (+120.f). Kẻ thù: Giảm X (-120.f).
    const float spacingX = isPlayerTeam ? 150.f : -150.f; 

    // Chỉ lấy quái vật còn sống
    vector<AnimatedMonster*> aliveMonsters;
    for(auto& am : team) if (am.monster.isAlive()) aliveMonsters.push_back(&am);

    if (aliveMonsters.empty()) return;

    // Sắp xếp tối đa 3 quái vật trên một hàng ngang
    for (size_t i = 0; i < aliveMonsters.size() && i < 3; ++i) {
        AnimatedMonster* am = aliveMonsters[i];
        
        // Vị trí mới: Base X + (chỉ số * khoảng cách X)
        sf::Vector2f targetPos = { basePos.x + i * spacingX, basePos.y };
        
        // Cập nhật vị trí
        if (am->currentPos.x == 0 && am->currentPos.y == 0) am->currentPos = targetPos;
        am->targetPos = targetPos;
        am->moveTime = 0.f; 
        am->moveDuration = 0.f;
    }

    // Đảm bảo quái vật đã chết không hiển thị
    for(auto& am : team) if (!am.monster.isAlive()) { am.targetPos = {-1000.f, -1000.f}; am.currentPos = {-1000.f, -1000.f}; }
}

void Battle::startRound() {
    state = BattleState::WAIT_INPUT;
    playerDice.clear();
    enemyDice.clear();
    isAllyTurn = true; 

    // Logic tạo xúc xắc (giữ nguyên)
    for (int i = 0; i < 2; ++i) {
        DiceInfo die;
        die.isUsed = false;
        int typeRoll = Utils::randomInt(0, 100);
        if (typeRoll < 40) { 
            die.type = DiceFace::Attack; die.value = Utils::randomInt(3, 8); die.description = "ATK";
        } else if (typeRoll < 70) { 
            die.type = DiceFace::Shield; die.value = Utils::randomInt(2, 6); die.description = "DEF";
        } else if (typeRoll < 85) { 
            die.type = DiceFace::BuffATK; die.value = Utils::randomInt(1, 3); die.description = "BUFF";
        } else { 
            die.type = DiceFace::RotateLeft; die.value = 0; die.description = "ROT";
        }
        playerDice.push_back(die);
        enemyDice.push_back(die);
    }

    // Cập nhật Animated Monsters (giữ nguyên)
    animatedEnemies.clear();
    for(const auto& m : enemies) animatedEnemies.push_back({m, {0.f, 0.f}, {0.f, 0.f}, 0.f, 0.f});
    calculateFormationPositions(true); // Cập nhật vị trí đồng minh
    calculateFormationPositions(false); // Cập nhật vị trí kẻ thù
    
    infoText.setString("LEFT TURN: Use [1] or [2]");
}

void Battle::playerSelectDie(int index) {
    if (state != BattleState::WAIT_INPUT) return;

    if (isAllyTurn) {
        if (index == 2 || index == 3) return; 
        if (index < 0 || index >= (int)playerDice.size()) return;
        if (playerDice[index].isUsed) return;

        executeDieEffect(playerDice[index], true);
        playerDice[index].isUsed = true;
        isAllyTurn = false; 
        infoText.setString("RIGHT TURN: Use [3] or [4]");
    } else {
        if (index == 0 || index == 1) return;
        int realIndex = index - 2; 
        if (realIndex < 0 || realIndex >= (int)enemyDice.size()) return;
        if (enemyDice[realIndex].isUsed) return;

        executeDieEffect(enemyDice[realIndex], false);
        enemyDice[realIndex].isUsed = true;
        isAllyTurn = true; 
        infoText.setString("LEFT TURN: Use [1] or [2]");
    }
    state = BattleState::EXECUTING;
    actionTimer = 1.0f;
}

// --- LOGIC HIỆU ỨNG XÚC XẮC (Giữ nguyên logic của bạn) ---
void Battle::executeDieEffect(DiceInfo& die, bool isPlayerAction) {
    vector<Monster>& actorTeam = isPlayerAction ? allies : enemies;
    vector<Monster>& targetTeam = isPlayerAction ? enemies : allies;
    vector<AnimatedMonster>& animActor = isPlayerAction ? animatedAllies : animatedEnemies;

    if (actorTeam.empty() || !actorTeam[0].isAlive()) return; 

    string actorName = animActor[0].monster.getName();
    int targetIdx = findFirstAlive(targetTeam);

    // Thông báo hành động
    string actionDesc = "";
    if (die.type == DiceFace::Attack) actionDesc = "Attack " + Utils::toString(die.value) + " DMG";
    else if (die.type == DiceFace::Shield) actionDesc = "Shield " + Utils::toString(die.value);
    else if (die.type == DiceFace::BuffATK) actionDesc = "Buff ATK";
    else actionDesc = "Rotate";

    infoText.setString((isPlayerAction ? "[LEFT] " : "[RIGHT] ") + actorName + " uses " + actionDesc);

    switch (die.type) {
        case DiceFace::Attack:
            if (targetIdx != -1) {
                int totalDmg = die.value; 
                targetTeam[targetIdx].takeDamage(totalDmg);
            }
            break;
        case DiceFace::Shield:
            actorTeam[0].addShield(die.value);
            break;
        case DiceFace::BuffATK:
            actorTeam[0].buffAttack(die.value);
            break;
        case DiceFace::RotateLeft:
            rotateTeam(isPlayerAction, true);
            break;
        default: break;
    }
}

// --- LOGIC UPDATE (Giữ nguyên logic của bạn) ---
void Battle::update(float dt) {
    auto updateAnim = [&](vector<AnimatedMonster>& list) {
        for (auto& am : list) {
            if (am.moveTime > 0) {
                float t = 1.f - (am.moveTime / am.moveDuration);
                am.currentPos.x = am.currentPos.x * (1.f - t) + am.targetPos.x * t;
                am.currentPos.y = am.currentPos.y * (1.f - t) + am.targetPos.y * t;
                am.moveTime -= dt;
                if (am.moveTime <= 0) am.currentPos = am.targetPos;
            } else am.currentPos = am.targetPos;
        }
    };
    updateAnim(animatedAllies);
    updateAnim(animatedEnemies);

    if (state == BattleState::EXECUTING) {
        if (actionTimer > 0) { actionTimer -= dt; return; }
        state = BattleState::CHECK_END;
    }
    else if (state == BattleState::CHECK_END) {
        for(size_t i=0; i<allies.size(); ++i) animatedAllies[i].monster = allies[i];
        for(size_t i=0; i<enemies.size(); ++i) animatedEnemies[i].monster = enemies[i];
        calculateFormationPositions(true);
        calculateFormationPositions(false);

        int aAlive = 0; for(auto& m : allies) if(m.isAlive()) aAlive++;
        int eAlive = 0; for(auto& m : enemies) if(m.isAlive()) eAlive++;

        if (aAlive == 0) { winFlag = false; state = BattleState::FINISHED; }
        else if (eAlive == 0) { winFlag = true; state = BattleState::FINISHED; }
        else {
            bool leftHasDice = false; for(auto& d : playerDice) if(!d.isUsed) leftHasDice = true;
            bool rightHasDice = false; for(auto& d : enemyDice) if(!d.isUsed) rightHasDice = true;

            if (!leftHasDice && !rightHasDice) {
                // Giả sử Monster có hàm endTurn() để reset buff/debuff
                for(auto& m : allies) m.endTurn();
                for(auto& m : enemies) m.endTurn();
                startRound();
            } else {
                state = BattleState::WAIT_INPUT;
            }
        }
    }
}

// --- LOGIC RENDER DICE (Giữ nguyên logic của bạn) ---
void Battle::renderDiceGroups(sf::RenderWindow& win) {
    float gap = 120.f;
    float yPos = 600.f; 

    auto drawDie = [&](float x, float y, DiceInfo& die, sf::Color outlineCol, string keyHint) {
        sf::RectangleShape box(sf::Vector2f(100.f, 100.f));
        box.setPosition(x, y);
        box.setOutlineThickness(3.f);
        box.setOutlineColor(outlineCol);

        if (die.isUsed) {
            box.setFillColor(sf::Color(50, 50, 50, 200));
        } else {
            switch(die.type) {
                case DiceFace::Attack: box.setFillColor(sf::Color(220, 60, 60)); break; 
                case DiceFace::Shield: box.setFillColor(sf::Color(60, 100, 220)); break; 
                case DiceFace::BuffATK: box.setFillColor(sf::Color(220, 220, 60)); break; 
                default: box.setFillColor(sf::Color::White); break;
            }
        }
        win.draw(box);

        string mainText = "";
        string subText = "";
        
        if (die.type == DiceFace::RotateLeft) {
            mainText = "ROT";
            subText = "Move";
        } else {
            mainText = Utils::toString(die.value); 
            if (die.type == DiceFace::Attack) subText = "DMG";
            else if (die.type == DiceFace::Shield) subText = "DEF";
            else if (die.type == DiceFace::BuffATK) subText = "BUFF";
        }

        sf::Text tVal(mainText, font, 36);
        tVal.setStyle(sf::Text::Bold);
        tVal.setFillColor(die.isUsed ? sf::Color(150, 150, 150) : sf::Color::Black);
        sf::FloatRect b = tVal.getLocalBounds();
        tVal.setOrigin(b.left + b.width/2.0f, b.top + b.height/2.0f);
        tVal.setPosition(x + 50.f, y + 40.f);
        win.draw(tVal);

        sf::Text tSub(subText, font, 16);
        tSub.setFillColor(die.isUsed ? sf::Color(150, 150, 150) : sf::Color(20, 20, 20));
        sf::FloatRect b2 = tSub.getLocalBounds();
        tSub.setOrigin(b2.left + b2.width/2.0f, b2.top + b2.height/2.0f);
        tSub.setPosition(x + 50.f, y + 75.f);
        win.draw(tSub);

        if (!die.isUsed && !keyHint.empty()) {
            sf::Text k(keyHint, font, 14);
            k.setFillColor(sf::Color::White);
            k.setOutlineColor(sf::Color::Black);
            k.setOutlineThickness(1.5f);
            k.setPosition(x + 5, y + 2);
            win.draw(k);
        }
    };

    float leftX = 200.f; 
    for (int i = 0; i < (int)playerDice.size(); ++i) {
        sf::Color border = isAllyTurn ? sf::Color::Cyan : sf::Color(100, 100, 100);
        string hint = isAllyTurn ? "[" + Utils::toString(i+1) + "]" : "";
        drawDie(leftX + i * gap, yPos, playerDice[i], border, hint);
    }

    float rightX = 850.f; 
    for (int i = 0; i < (int)enemyDice.size(); ++i) {
        sf::Color border = !isAllyTurn ? sf::Color::Red : sf::Color(100, 100, 100);
        string hint = !isAllyTurn ? "[" + Utils::toString(i+3) + "]" : "";
        drawDie(rightX + i * gap, yPos, enemyDice[i], border, hint);
    }
}

// --- LOGIC RENDER MONSTER (Giữ nguyên logic của bạn) ---
void Battle::render(sf::RenderWindow& win) {
    win.draw(backgroundSprite);

    // Render Allies
    for (int i = 0; i < (int)animatedAllies.size(); ++i) {
        if (!animatedAllies[i].monster.isAlive()) continue;
        string path = animatedAllies[i].monster.getSpritePath();
        if (textureCache.find(path) == textureCache.end()) {
             // Thử tải texture một lần
             if (!textureCache[path].loadFromFile(path)) continue; 
        }
        
        sf::Sprite sp(textureCache[path]);
        sf::FloatRect bounds = sp.getLocalBounds();
        sp.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        sp.setScale(0.35f, 0.35f); 
        sp.setPosition(animatedAllies[i].currentPos);
        if (i == 0) sp.setColor(sf::Color::White); else sp.setColor(sf::Color(150, 150, 150));
        win.draw(sp);

        string status = "";
        int sh = animatedAllies[i].monster.getShield();
        if (sh > 0) status += " (Shld:" + Utils::toString(sh) + ")";
        
        sf::Text t(animatedAllies[i].monster.getName() + "\nHP: " + Utils::toString(animatedAllies[i].monster.getHP()) + status, font, 16);
        t.setOutlineColor(sf::Color::Black); t.setOutlineThickness(1.5f); t.setOrigin(t.getLocalBounds().width/2, 0);
        t.setPosition(animatedAllies[i].currentPos.x, animatedAllies[i].currentPos.y - 110);
        t.setFillColor((sh>0)?sf::Color::Yellow : sf::Color::White);
        win.draw(t);
    }

    // Render Enemies
    for (int i = 0; i < (int)animatedEnemies.size(); ++i) {
        if (!animatedEnemies[i].monster.isAlive()) continue;
        string path = animatedEnemies[i].monster.getSpritePath();
        if (textureCache.find(path) == textureCache.end()) {
             // Thử tải texture một lần
             if (!textureCache[path].loadFromFile(path)) continue;
        }
        sf::Sprite sp(textureCache[path]);
        sf::FloatRect bounds = sp.getLocalBounds();
        sp.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        sp.setScale(-0.35f, 0.35f); // Scale âm để lật sprite kẻ thù
        sp.setPosition(animatedEnemies[i].currentPos);
        if (i == 0) sp.setColor(sf::Color::White); else sp.setColor(sf::Color(150, 150, 150));
        win.draw(sp);

        string status = "";
        int sh = animatedEnemies[i].monster.getShield();
        if (sh > 0) status += " (Shld:" + Utils::toString(sh) + ")";

        sf::Text t("HP: " + Utils::toString(animatedEnemies[i].monster.getHP()) + status, font, 16);
        t.setOutlineColor(sf::Color::Black); t.setOutlineThickness(1.5f); t.setOrigin(t.getLocalBounds().width/2, 0);
        t.setPosition(animatedEnemies[i].currentPos.x, animatedEnemies[i].currentPos.y - 110);
        t.setFillColor((sh>0)?sf::Color::Yellow : sf::Color(255, 100, 100));
        win.draw(t);
    }

    win.draw(infoText);
    renderDiceGroups(win); 
}

bool Battle::isFinished() const { return state == BattleState::FINISHED; }
bool Battle::playerWon() const { return winFlag; }
BattleState Battle::getState() const { return state; }

// --- LOGIC ROTATE (Giữ nguyên logic của bạn) ---
void Battle::rotateTeam(bool isPlayerTeam, bool left) {
    vector<Monster> &team = isPlayerTeam ? allies : enemies;
    vector<AnimatedMonster> &animatedTeam = isPlayerTeam ? animatedAllies : animatedEnemies;
    if (team.empty()) return;

    if (left) {
        Monster firstMonster = team[0];
        for (size_t i = 0; i + 1 < team.size(); ++i) team[i] = team[i+1];
        team[team.size()-1] = firstMonster;
    } else {
        Monster lastMonster = team[team.size()-1];
        for (size_t i = team.size()-1; i > 0; --i) team[i] = team[i-1];
        team[0] = lastMonster;
    }
    for (size_t i = 0; i < team.size(); ++i) {
        animatedTeam[i].monster = team[i];
        // Thiết lập animation di chuyển
        animatedTeam[i].moveTime = 0.2f; 
        animatedTeam[i].moveDuration = 0.2f;
    }
    // Cập nhật lại targetPos sau khi xoay
    calculateFormationPositions(isPlayerTeam);
}
