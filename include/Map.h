#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include "Node.h"

class Map {
public:
    Map();
    void generateDefaultLayout();
    void render(sf::RenderWindow& win, sf::Font& font);
    
    // Hàm xử lý click chuột: Trả về true nếu chọn được một node hợp lệ
    bool clickNode(const sf::Vector2f& mousePos);
    
    void movePlayerToNode(int id);
    Node* getNodeById(int id);
    std::vector<Node>& getNodes() { return nodes; }
    int getCurrentNodeId() const; // Node đang đứng (hiện tại)

    // --- MỚI: Quản lý tiến độ ---
    void unlockNextNode(); // Gọi hàm này khi thắng trận
    int getSelectedNodeId() const { return selectedNodeId; } // Node đang chọn (highlight)
    bool canEnterSelectedNode() const; // Kiểm tra có được vào node đang chọn không
    // ---------------------------

private:
    std::vector<Node> nodes;
    int currentNodeId; // Node người chơi đang đứng (tiến độ thực tế)
    
    // --- MỚI ---
    int maxUnlockedNodeId; // Node xa nhất đã mở khóa
    int selectedNodeId;    // Node người chơi vừa click chuột vào (để chuẩn bị Enter)
    // -----------

    std::map<NodeType, sf::Texture> nodeTextures;
    void loadTextures();
};