#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

// --- DUSMAN HEDEF YAPISI (YENI) ---
struct EnemyTarget {
    sf::RectangleShape shape;
    std::string id;
    sf::Color normalColor;
    sf::Color hoverColor;

    EnemyTarget(std::string _id, float x, float y) {
        id = _id;
        // Dusman boyutu (Orn: 100x150 piksel)
        shape.setSize({100.f, 150.f}); 
        shape.setPosition({x, y});
        
        // Renk ayarlari
        normalColor = sf::Color(50, 0, 0);   // Normalde cok koyu kirmizi
        hoverColor = sf::Color(255, 50, 50); // Uzerine gelince parlak kirmizi
        
        shape.setFillColor(normalColor);
        shape.setOutlineThickness(2.f);
        shape.setOutlineColor(sf::Color::Black);
    }

    // Hover Durumunu Guncelle
    void update(sf::Vector2f mousePos) {
        if (shape.getGlobalBounds().contains(mousePos)) {
            shape.setFillColor(hoverColor); // Parlat
        } else {
            shape.setFillColor(normalColor); // Normale don
        }
    }

    // Tiklandi mi?
    bool isClicked(sf::Vector2f mousePos) {
        return shape.getGlobalBounds().contains(mousePos);
    }
};

// --- TIKLANABILIR YAZI YAPISI ---
struct DialogueOption {
    sf::Text text;
    std::string id;

    DialogueOption(sf::Font& font, std::string _text, std::string _id, float x, float y) 
        : text(font) 
    {
        id = _id;
        text.setString(_text);
        text.setCharacterSize(18);
        text.setFillColor(sf::Color::White);
        text.setPosition({x, y}); 
    }

    bool isHovered(sf::Vector2f mousePos) {
        return text.getGlobalBounds().contains(mousePos);
    }
};

// --- BUTON YAPISI ---
struct Button {
    sf::RectangleShape shape;
    std::string id;
    
    Button(std::string _id, sf::Vector2f size, sf::Vector2f position, sf::Color color) {
        id = _id;
        shape.setSize(size);
        shape.setPosition(position);
        shape.setFillColor(color);
        shape.setOutlineThickness(-1.f); 
        shape.setOutlineColor(sf::Color::Black);
    }

    bool isClicked(sf::Vector2i mousePos) {
        return shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode({960, 540}), "RPG Prototype - Enemies Added");
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(true);

    sf::Font font;
    if (!font.openFromFile("font.ttf")) {
        std::cerr << "HATA: font.ttf dosyasi bulunamadi!" << std::endl;
        return -1;
    }

    // --- LAYOUT HESAPLARI ---
    float leftWidth = 560.f; 
    float rightWidth = 200.f; 
    float totalGameWidth = leftWidth + rightWidth; 
    float offsetX = (960.f - totalGameWidth) / 2.f; 
    
    float gameStartX = offsetX;
    float splitY = 380.f;

    // --- PANELLER ---
    sf::RectangleShape redPanel({leftWidth, splitY});
    redPanel.setPosition({gameStartX, 0.f}); 
    redPanel.setFillColor(sf::Color(20, 10, 10)); // Zindan arkaplani (Cok koyu)

    sf::RectangleShape greenPanel({rightWidth, splitY});
    greenPanel.setPosition({gameStartX + leftWidth, 0.f}); 
    greenPanel.setFillColor(sf::Color(30, 100, 30)); 

    sf::RectangleShape bluePanel({leftWidth, 540.f - splitY});
    bluePanel.setPosition({gameStartX, splitY}); 
    bluePanel.setFillColor(sf::Color(30, 30, 100)); 


    // --- DUSMANLARI EKLE (YENI KISIM) ---
    std::vector<EnemyTarget> enemies;
    
    // Kirmizi panelin merkezini bulalim
    float centerX = gameStartX + (leftWidth / 2.f);
    float centerY = splitY / 2.f;

    // Dusman 1 (Soldaki)
    // Merkezden 110 piksel sola, biraz yukari
    enemies.emplace_back("GOBLIN_WARRIOR", centerX - 110.f, centerY - 50.f);

    // Dusman 2 (Sagdaki)
    // Merkezden 10 piksel saga, biraz yukari
    enemies.emplace_back("GOBLIN_MAGE", centerX + 10.f, centerY - 50.f);


    // --- DIYALOG ---
    sf::Text npcText(font);
    npcText.setString("Gatekeeper:\n'Beware! Two enemies block your path.'");
    npcText.setCharacterSize(18);
    npcText.setFillColor(sf::Color(200, 200, 200)); 
    npcText.setPosition({bluePanel.getPosition().x + 20.f, bluePanel.getPosition().y + 15.f}); 

    std::vector<DialogueOption> options;
    options.emplace_back(font, "> Fight them!", "OPT_FIGHT", bluePanel.getPosition().x + 20.f, bluePanel.getPosition().y + 60.f);
    options.emplace_back(font, "> Try to sneak past.", "OPT_SNEAK", bluePanel.getPosition().x + 20.f, bluePanel.getPosition().y + 90.f);

    // --- BUTONLAR ---
    std::vector<Button> buttons;
    float colWidth = rightWidth / 2.f; 
    float areaHeight = 540.f - splitY;       
    float yellowH = areaHeight / 4.f;
    float actionStartX = gameStartX + leftWidth;
    float actionStartY = splitY;

    for (int i = 0; i < 4; i++) {
        buttons.push_back(Button("BTN_" + std::to_string(i), {colWidth, yellowH}, {actionStartX, actionStartY + (i * yellowH)}, sf::Color(180, 160, 0)));
    }
    float purpleH = areaHeight / 2.f; 
    buttons.push_back(Button("MAP", {colWidth, purpleH}, {actionStartX + colWidth, actionStartY}, sf::Color(100, 0, 100)));
    buttons.push_back(Button("INV", {colWidth, purpleH}, {actionStartX + colWidth, actionStartY + purpleH}, sf::Color(80, 0, 80)));


    // --- DONGU ---
    while (window.isOpen()) {
        
        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);

        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            
            if (const auto* mousePress = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePress->button == sf::Mouse::Button::Left) {
                    sf::Vector2i clickPos = mousePress->position;
                    
                    // 1. DUSMANLARA TIKLANDI MI? (YENI)
                    for (auto& enemy : enemies) {
                        if (enemy.isClicked(static_cast<sf::Vector2f>(clickPos))) {
                            std::cout << ">>> DUSMAN SECILDI: " << enemy.id << std::endl;
                        }
                    }

                    // 2. YAZILARA TIKLANDI MI?
                    for (auto& opt : options) {
                        if (opt.isHovered(static_cast<sf::Vector2f>(clickPos))) {
                            std::cout << ">>> DIYALOG SECILDI: " << opt.id << std::endl;
                        }
                    }

                    // 3. BUTONLARA TIKLANDI MI?
                    for (auto& btn : buttons) {
                        if (btn.isClicked(clickPos)) {
                            std::cout << ">>> BUTON: " << btn.id << std::endl;
                        }
                    }
                }
            }
        }

        // --- UPDATE ---
        
        // Diyalog Hover Efekti
        for (auto& opt : options) {
            if (opt.isHovered(mousePos)) {
                opt.text.setFillColor(sf::Color::Yellow);
                opt.text.setStyle(sf::Text::Underlined);
            } else {
                opt.text.setFillColor(sf::Color::White);
                opt.text.setStyle(sf::Text::Regular);
            }
        }

        // DUSMAN HOVER EFEKTI (YENI)
        for (auto& enemy : enemies) {
            enemy.update(mousePos);
        }

        window.clear(sf::Color::Black);

        // Panelleri Ciz
        window.draw(redPanel);
        // DUSMANLARI CIZ (Kirmizi panelin ustune gelmeli)
        for (const auto& enemy : enemies) window.draw(enemy.shape);

        window.draw(greenPanel);
        window.draw(bluePanel);

        // Butonlari Ciz
        for (const auto& btn : buttons) window.draw(btn.shape);

        // Yazilari Ciz
        window.draw(npcText);
        for (const auto& opt : options) window.draw(opt.text);

        window.display();
    }

    return 0;
}