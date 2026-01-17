#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

// --- RENK AYARI ---
const sf::Color BORDEAUX_COLOR = sf::Color(110, 0, 0); 

struct EnemyTarget {
    sf::RectangleShape shape;
    std::string id;
    sf::Color normalColor;
    sf::Color hoverColor;
    EnemyTarget(std::string _id, float x, float y) {
        id = _id;
        shape.setSize({100.f, 150.f}); 
        shape.setPosition({x, y});
        normalColor = sf::Color(50, 0, 0);   
        hoverColor = sf::Color(255, 50, 50); 
        shape.setFillColor(normalColor);
        shape.setOutlineThickness(2.f);
        shape.setOutlineColor(sf::Color::Black);
    }
    void update(sf::Vector2f mousePos) {
        if (shape.getGlobalBounds().contains(mousePos)) shape.setFillColor(hoverColor); 
        else shape.setFillColor(normalColor); 
    }
    bool isClicked(sf::Vector2f mousePos) {
        return shape.getGlobalBounds().contains(mousePos);
    }
};

struct DialogueOption {
    sf::Text text;
    std::string id;
    DialogueOption(sf::Font& font, std::string _text, std::string _id, float x, float y) : text(font) {
        id = _id;
        text.setString(_text);
        text.setCharacterSize(16);
        text.setFillColor(BORDEAUX_COLOR); 
        text.setPosition({x, y}); 
    }
    bool isHovered(sf::Vector2f mousePos) { return text.getGlobalBounds().contains(mousePos); }
};

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
    sf::RenderWindow window(sf::VideoMode({960, 540}), "RPG - Statbox Added");
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(true);

    sf::Font font;
    if (!font.openFromFile("font.ttf")) return -1;

    // --- TEXTURE YUKLEME ---
    
    // 1. Diyalog Kutusu
    sf::Texture dialogTexture;
    if (!dialogTexture.loadFromFile("textures/Textbox[Final].png")) {
        std::cerr << "HATA: Textbox resmi bulunamadi!" << std::endl;
        return -1;
    }

    // 2. Stat Kutusu (YENI)
    sf::Texture statTexture;
    if (!statTexture.loadFromFile("textures/Statbox[Final].png")) {
        std::cerr << "HATA: Statbox resmi bulunamadi!" << std::endl;
        return -1;
    }

    // --- LAYOUT ---
    float leftWidth = 560.f; 
    float rightWidth = 200.f; 
    float totalGameWidth = leftWidth + rightWidth; 
    float offsetX = (960.f - totalGameWidth) / 2.f; 
    float gameStartX = offsetX;
    float splitY = 380.f;

    // --- PANELLER ---
    
    // A) POV (Kirmizi Alan) - Hala Shape (Yakinda buna da resim cizersin)
    sf::RectangleShape redPanel({leftWidth, splitY});
    redPanel.setPosition({gameStartX, 0.f}); 
    redPanel.setFillColor(sf::Color(20, 10, 10)); 

    // B) STAT KUTUSU (SPRITE) - Eski GreenPanel yerine gecti
    sf::Sprite statSprite(statTexture);
    statSprite.setPosition({gameStartX + leftWidth, 0.f}); // Sag ust koseye yerlestir

    // Olcekleme: (Hedef Genislik / Resim Genisligi)
    float statScaleX = rightWidth / static_cast<float>(statTexture.getSize().x);
    float statScaleY = splitY / static_cast<float>(statTexture.getSize().y);
    statSprite.setScale({statScaleX, statScaleY});


    // C) DIYALOG KUTUSU (SPRITE)
    sf::Sprite dialogSprite(dialogTexture);
    dialogSprite.setPosition({gameStartX, splitY});
    float diaScaleX = leftWidth / static_cast<float>(dialogTexture.getSize().x);      
    float diaScaleY = (540.f - splitY) / static_cast<float>(dialogTexture.getSize().y); 
    dialogSprite.setScale({diaScaleX, diaScaleY});

    
    // --- OYUN ICI NESNELER ---
    std::vector<EnemyTarget> enemies;
    float centerX = gameStartX + (leftWidth / 2.f);
    float centerY = splitY / 2.f;
    enemies.emplace_back("GOBLIN_WARRIOR", centerX - 110.f, centerY - 50.f);
    enemies.emplace_back("GOBLIN_MAGE", centerX + 10.f, centerY - 50.f);

    // --- YAZILAR ---

    // 1. STAT YAZILARI (YENI - Kutunun icine ornek yazi)
    sf::Text statText(font);
    statText.setString("HERO\n\nHP: 100\nMP: 40\n\nSTR: 12\nDEF: 8\n\nGOLD: 50");
    statText.setCharacterSize(16);
    statText.setFillColor(BORDEAUX_COLOR);
    // Stat kutusunun icine, biraz iceri (padding) vererek yerlestiriyoruz
    statText.setPosition({statSprite.getPosition().x + 35.f, statSprite.getPosition().y + 40.f});


    // 2. DIYALOG YAZILARI (Senin ayarladigin padding degerleri)
    float textPadX = 45.f; // <--- SENIN AYARIN
    float textPadY = 30.f; // <--- SENIN AYARIN

    sf::Text npcText(font);
    npcText.setString("Gatekeeper:\n'You look stronger properly framed.'");
    npcText.setCharacterSize(16);
    npcText.setFillColor(BORDEAUX_COLOR); 
    npcText.setPosition({dialogSprite.getPosition().x + textPadX, dialogSprite.getPosition().y + textPadY}); 

    std::vector<DialogueOption> options;
    options.emplace_back(font, "> Ready for battle.", "OPT_YES", dialogSprite.getPosition().x + textPadX, dialogSprite.getPosition().y + textPadY + 45.f);
    options.emplace_back(font, "> Check stats first.", "OPT_NO", dialogSprite.getPosition().x + textPadX, dialogSprite.getPosition().y + textPadY + 70.f);

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
                    for (auto& enemy : enemies) { if (enemy.isClicked(static_cast<sf::Vector2f>(clickPos))) std::cout << "DUSMAN: " << enemy.id << std::endl; }
                    for (auto& opt : options) { if (opt.isHovered(static_cast<sf::Vector2f>(clickPos))) std::cout << "DIYALOG: " << opt.id << std::endl; }
                    for (auto& btn : buttons) { if (btn.isClicked(clickPos)) std::cout << "BUTON: " << btn.id << std::endl; }
                }
            }
        }

        for (auto& opt : options) {
            if (opt.isHovered(mousePos)) { 
                opt.text.setFillColor(sf::Color::Red); 
                opt.text.setStyle(sf::Text::Underlined); 
            } else { 
                opt.text.setFillColor(BORDEAUX_COLOR); 
                opt.text.setStyle(sf::Text::Regular); 
            }
        }
        for (auto& enemy : enemies) enemy.update(mousePos);

        window.clear(sf::Color::Black); 

        // 1. Zindan (Shape)
        window.draw(redPanel);
        for (const auto& enemy : enemies) window.draw(enemy.shape);
        
        // 2. Stat Kutusu (Sprite) - YESIL KUTU GITTI, RESIM GELDI
        window.draw(statSprite);
        window.draw(statText); // Icine yazi
        
        // 3. Diyalog Kutusu (Sprite)
        window.draw(dialogSprite); 

        // 4. Arayuz
        for (const auto& btn : buttons) window.draw(btn.shape);
        window.draw(npcText);
        for (const auto& opt : options) window.draw(opt.text);

        window.display();
    }

    return 0;
}