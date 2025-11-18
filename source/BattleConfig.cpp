#include "BattleConfig.h"

using namespace std;

vector<StageInfo> BattleConfig::stages;
StageInfo BattleConfig::boss;

void BattleConfig::initStages() {
    stages.clear();
    stages.resize(6); // 6 màn chơi
    
    // SỬA LỖI: Thêm "../" vào tất cả đường dẫn sprite
    // Stage 0: Gặp Boglin yếu
    stages[0].enemies = { 
        Monster("Escabal", 15, 5, "../assets/sprites/Escabal.png"), 
        Monster("Boglin", 15, 5, "../assets/sprites/Boglin.png") 
    };

    // Stage 1: Gặp Beloid
    stages[1].enemies = { 
        Monster("Escabal", 20, 5, "../assets/sprites/Escabal.png") 
    };

    // Stage 2: Hỗn hợp
    stages[2].enemies = { 
        Monster("Carascohl", 25, 5, "../assets/sprites/Carascohl.png"), 
        Monster("Boglin", 15, 5, "../assets/sprites/Boglin.png") 
    };

    // Stage 3: Địch mạnh hơn
    stages[3].enemies = { 
        Monster("Escabal", 35, 5, "../assets/sprites/Escabal.png"), 
        Monster("Boglin", 22, 5, "../assets/sprites/Boglin.png") 
    };

    // Stage 4
    stages[4].enemies = { 
        Monster("Carascohl", 15, 7, "../assets/sprites/Carascohl.png") 
    };

    // Stage 5
    stages[5].enemies = { 
        Monster("Escabal", 20, 5, "../assets/sprites/Escabal.png"),
        Monster("Carascohl", 25, 5, "../assets/sprites/Carascohl.png"), 
        Monster("Akama", 15, 5, "../assets/sprites/Akama.png") 
    };
    
    // BOSS: Akama
    boss.enemies = { 
        Monster("Gargullet (Boss)", 200, 5, "../assets/sprites/Gargullet.png") 
    };
}