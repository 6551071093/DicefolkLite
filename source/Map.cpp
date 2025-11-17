#include "Map.h"
#include "Utils.h"
#include <iostream>

using namespace std;

Map::Map() : currentNodeId(0), maxUnlockedNodeId(0), selectedNodeId(0) {
    loadTextures();
}

void Map::loadTextures() {
    // Load ảnh cho các icon trên bản đồ
    // SỬA LỖI: Thêm "../"
    if (!nodeTextures[NodeType::Battle].loadFromFile("../assets/sprites/Battle.png")) {
        // Fallback nếu thiếu ảnh
    }
    nodeTextures[NodeType::Boss].loadFromFile("../assets/sprites/Boss.png");
    nodeTextures[NodeType::Bonfire].loadFromFile("../assets/sprites/Bonfire.png");
    nodeTextures[NodeType::Berry].loadFromFile("../assets/sprites/Broodberries.png");
    nodeTextures[NodeType::Shop].loadFromFile("../assets/sprites/Abandoned_tent.png");
    nodeTextures[NodeType::Empty].loadFromFile("../assets/sprites/Battle_cleared.png"); 
}

void Map::generateDefaultLayout() {
    nodes.clear();
    int id = 0;
    
    // Tạo 6 màn chơi nằm ngang
    for (int i = 0; i < 6; ++i) {
        Node n;
        n.id = id++;
        n.pos = { 200.f + i * 160.f, 360.f }; // Cách nhau 160px
        
        // Màn cuối là Boss, còn lại là Battle
        if (i == 5) n.type = NodeType::Boss;
        else n.type = NodeType::Battle;

        n.stageID = i; 
        nodes.push_back(n);
    }
    
    // Set mặc định
    currentNodeId = 0;
    maxUnlockedNodeId = 0; // Ban đầu chỉ chơi được màn 0
    selectedNodeId = 0;    // Mặc định chọn màn 0
}

// --- LOGIC MỞ KHÓA MÀN MỚI ---
void Map::unlockNextNode() {
    // Chỉ mở nếu chưa phá đảo (chưa hết node)
    if (maxUnlockedNodeId < (int)nodes.size() - 1) {
        maxUnlockedNodeId++;
    }
    // Tự động chuyển con trỏ chọn sang màn mới nhất
    selectedNodeId = maxUnlockedNodeId;
    currentNodeId = maxUnlockedNodeId;
}

// Kiểm tra xem màn đang chọn có vào được không
bool Map::canEnterSelectedNode() const {
    return selectedNodeId <= maxUnlockedNodeId;
}

// Xử lý click chuột
bool Map::clickNode(const sf::Vector2f& mPos) {
    for (auto &n : nodes) {
        // Tăng vùng click lên 45px cho dễ bấm
        if (Utils::pointInCircle(n.pos, 45.f, mPos)) {
            // Chỉ cho phép chọn nếu màn đó ĐÃ MỞ KHÓA
            if (n.id <= maxUnlockedNodeId) {
                selectedNodeId = n.id;
                return true;
            }
        }
    }
    return false;
}

void Map::render(sf::RenderWindow& win, sf::Font& font) {
    // 1. VẼ ĐƯỜNG NỐI
    sf::Vertex line[2];
    for (size_t i = 0; i < nodes.size() - 1; ++i) {
        line[0].position = nodes[i].pos;
        line[1].position = nodes[i+1].pos;
        
        // Nếu đường đi đã mở: Màu Trắng. Chưa mở: Màu Xám tối
        if (i < (size_t)maxUnlockedNodeId) {
            line[0].color = sf::Color::White;
            line[1].color = sf::Color::White;
        } else {
            line[0].color = sf::Color(80, 80, 80);
            line[1].color = sf::Color(80, 80, 80);
        }
        win.draw(line, 2, sf::Lines);
    }

    // 2. VẼ CÁC NODE
    for (auto &n : nodes) {
        sf::Sprite spr;
        
        if (nodeTextures.count(n.type)) {
            spr.setTexture(nodeTextures[n.type]);
            spr.setOrigin(spr.getLocalBounds().width / 2.f, spr.getLocalBounds().height / 2.f);
            spr.setPosition(n.pos);
            spr.setScale(0.5f, 0.5f);

            // --- XỬ LÝ HIỆU ỨNG MÀU SẮC ---
            if (n.id > maxUnlockedNodeId) {
                // Chưa mở khóa: Đen thui + Mờ
                spr.setColor(sf::Color(50, 50, 50, 150)); 
            } else {
                if (n.id == selectedNodeId) {
                    // Đang chọn: Sáng rực + Phóng to nhẹ
                    spr.setColor(sf::Color::White);
                    spr.setScale(0.6f, 0.6f);
                    
                    // Vẽ vòng tròn highlight bao quanh
                    sf::CircleShape ring(50.f);
                    ring.setOrigin(50.f, 50.f);
                    ring.setPosition(n.pos);
                    ring.setFillColor(sf::Color::Transparent);
                    ring.setOutlineColor(sf::Color::Cyan);
                    ring.setOutlineThickness(4.f);
                    win.draw(ring);
                } else {
                    // Đã mở nhưng không chọn: Hơi xám
                    spr.setColor(sf::Color(200, 200, 200));
                }
            }
            win.draw(spr);
        } 
        else {
            // Fallback nếu không có ảnh: Vẽ hình tròn
            sf::CircleShape c(20.f);
            c.setOrigin(20.f, 20.f);
            c.setPosition(n.pos);
            c.setFillColor(n.id <= maxUnlockedNodeId ? sf::Color::Yellow : sf::Color::Red);
            win.draw(c);
        }
        
        // Vẽ số màn chơi (Stage ID) bên dưới
        if (n.id <= maxUnlockedNodeId) {
            sf::Text t(Utils::toString(n.id + 1), font, 20);
            t.setOrigin(t.getLocalBounds().width/2, 0);
            t.setPosition(n.pos.x, n.pos.y + 35);
            t.setOutlineColor(sf::Color::Black);
            t.setOutlineThickness(2.f);
            win.draw(t);
        }
    }
}

// Các hàm getter
void Map::movePlayerToNode(int id) { currentNodeId = id; }
Node* Map::getNodeById(int id) { for(auto& n : nodes) if(n.id == id) return &n; return nullptr; }
int Map::getCurrentNodeId() const { return currentNodeId; }

// [QUAN TRỌNG] Đã xóa dòng getSelectedNodeId ở đây vì nó đã có trong file .h