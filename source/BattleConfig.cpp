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
        Monster("Escabal", 10, 3, "../assets/sprites/Escabal.png"), 
        Monster("Boglin", 15, 2, "../assets/sprites/Boglin.png") 
    };

    // Stage 1: Gặp Beloid
    stages[1].enemies = { 
        Monster("Escabal", 20, 3, "../assets/sprites/Escabal.png") 
    };

    // Stage 2: Hỗn hợp
    stages[2].enemies = { 
        Monster("Carascohl", 25, 2, "../assets/sprites/Carascohl.png"), 
        Monster("Boglin", 15, 3, "../assets/sprites/Boglin.png") 
    };

    // Stage 3: Địch mạnh hơn
    stages[3].enemies = { 
        Monster("Escabal", 35, 3, "../assets/sprites/Escabal.png"), 
        Monster("Beloid C", 22, 5, "../assets/sprites/Beloid_C.png") 
    };

    // Stage 4
    stages[4].enemies = { 
        Monster("Carascohl", 15, 4, "../assets/sprites/Carascohl.png") 
    };

    // Stage 5
    stages[5].enemies = { 
        Monster("Escabal", 20, 5, "../assets/sprites/Escabal.png"), 
        Monster("Beloid_A", 15, 5, "../assets/sprites/Beloid_A.png") 
    };
    
    // BOSS: Akama
    boss.enemies = { 
        Monster("Akama (Boss)", 200, 5, "../assets/sprites/Akama.png") 
    };
}