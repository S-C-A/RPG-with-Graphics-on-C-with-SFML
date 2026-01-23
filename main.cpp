#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

#include "Player.h" 

// --- OYUN DURUMLARI (SADECE OYNANIS) ---
// Envanteri buradan sildik, cunku o ayri bir katman olacak.
enum class GameState {
    EXPLORING,      // Gezinti Modu
    COMBAT          // Savas Modu
};

// --- RENK AYARI ---
const sf::Color BORDEAUX_COLOR = sf::Color(110, 0, 0); 

// --- STATLARI EKRANA YAZ ---
void updateStatText(sf::Text& text, Player& player) {
    std::string content;
    content += "   KEYBEARER\n";
    content += "     (Lvl " + std::to_string(player.getLvl()) + ")\n\n";
    content += "HP:   " + std::to_string(player.getHp()) + "/" + std::to_string(player.getMaxHp()) + "\n\n";
    content += "ATK:  " + std::to_string(player.getAtk()) + "\n";
    content += "DEF:  " + std::to_string(player.getDef()) + "\n\n";
    content += "GOLD: " + std::to_string(player.getGold()) + "\n";
    content += "EXP:  " + std::to_string(player.getExp()) + "\n\n";
    content += "Weapon\n[" + player.getWeaponName() + "]\n\n";
    content += "Armor\n[" + player.getArmorName() + "]";
    text.setString(content);
}

// --- ENVANTER LISTELEME ---
void updateInventoryText(sf::Text& text, Player& player) {
    std::string content = "--- BACKPACK ---\n\n";
    const auto& inv = player.getInventory();

    for (int i = 0; i < 10; ++i) {
        content += "[" + std::to_string(i + 1) + "] ";
        if (i < inv.size()) {
            content += inv[i]->getName();
        } else {
            content += "Empty";
        }
        content += "\n";
    }
    text.setString(content);
}

// --- DIYALOG KUTUSU GUNCELLE ---
void updateDialogue(sf::Text& text, std::string message) {
    text.setString("System:\n" + message);
}

// --- DUSMAN ---
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

// --- DIYALOG SECENEK ---
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

// --- BUTON ---
struct Button {
    sf::Sprite sprite; 
    sf::Text label;    
    std::string id;
    sf::Vector2f baseScale; 

    Button(std::string _id, std::string _labelText, const sf::Texture& texture, sf::Font& font, sf::Vector2f position, sf::Vector2f targetSize) 
        : sprite(texture), label(font) 
    {
        id = _id;
        sf::Vector2u texSize = texture.getSize();
        sprite.setOrigin({static_cast<float>(texSize.x) / 2.f, static_cast<float>(texSize.y) / 2.f});
        sprite.setPosition({position.x + targetSize.x / 2.f, position.y + targetSize.y / 2.f});

        float scaleX = targetSize.x / static_cast<float>(texSize.x);
        float scaleY = targetSize.y / static_cast<float>(texSize.y);
        baseScale = {scaleX, scaleY}; 
        sprite.setScale(baseScale);

        label.setString(_labelText);
        label.setCharacterSize(14); 
        label.setFillColor(BORDEAUX_COLOR);
        
        if (!_labelText.empty()) {
            sf::FloatRect textBounds = label.getLocalBounds();
            label.setOrigin({
                textBounds.position.x + textBounds.size.x / 2.0f, 
                textBounds.position.y + textBounds.size.y / 2.0f
            });
            label.setPosition(sprite.getPosition());
        }
    }

    void update(sf::Vector2f mousePos, bool isMousePressed) {
        if (sprite.getGlobalBounds().contains(mousePos)) {
            if (isMousePressed) sprite.setScale({baseScale.x * 0.85f, baseScale.y * 0.85f});
            else sprite.setScale({baseScale.x * 0.95f, baseScale.y * 0.95f});
        } else {
            sprite.setScale(baseScale);
        }
    }
    
    void setLabelText(std::string newText) {
        label.setString(newText);
        sf::FloatRect textBounds = label.getLocalBounds();
        label.setOrigin({
            textBounds.position.x + textBounds.size.x / 2.0f, 
            textBounds.position.y + textBounds.size.y / 2.0f
        });
        label.setPosition(sprite.getPosition());
    }

    bool isClicked(sf::Vector2f mousePos) {
        return sprite.getGlobalBounds().contains(mousePos);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
        if (label.getString().getSize() > 0) window.draw(label);
    }
};

void updateButtonLabels(std::vector<Button>& buttons, GameState state) {
    if (state == GameState::EXPLORING) {
        buttons[0].setLabelText("NORTH");
        buttons[1].setLabelText("WEST");
        buttons[2].setLabelText("EAST");
        buttons[3].setLabelText("SOUTH");
    } 
    else if (state == GameState::COMBAT) {
        buttons[0].setLabelText("ATK");
        buttons[1].setLabelText("DEF");
        buttons[2].setLabelText("ITEM");
        buttons[3].setLabelText("RUN");
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "RPG - Inventory Overlay", sf::State::Fullscreen);
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(true);
    sf::View gameView(sf::FloatRect({0.f, 0.f}, {960.f, 540.f}));
    window.setView(gameView);

    // --- TEMEL DEGISKENLER ---
    GameState currentState = GameState::EXPLORING;
    bool isInventoryOpen = false; // <--- YENI: Envanter acik mi bayragi

    Player hero; 
    sf::Font font;
    if (!font.openFromFile("font.ttf")) return -1;

    // --- TEXTURE YUKLEME ---
    sf::Texture dialogTexture; if (!dialogTexture.loadFromFile("textures/Textbox[Final].png")) return -1;
    sf::Texture statTexture; if (!statTexture.loadFromFile("textures/Statbox[Final].png")) return -1;
    sf::Texture buttonTexture; if (!buttonTexture.loadFromFile("textures/Button[Final].png")) return -1;
    sf::Texture inventoryTexture; if (!inventoryTexture.loadFromFile("textures/Inventory[Final].png")) return -1;
    sf::Texture mapTexture; if (!mapTexture.loadFromFile("textures/Map[Final].png")) { mapTexture = inventoryTexture; }

    // --- LAYOUT ---
    float leftWidth = 560.f; 
    float rightWidth = 200.f; 
    float totalGameWidth = leftWidth + rightWidth; 
    float offsetX = (960.f - totalGameWidth) / 2.f; 
    float gameStartX = offsetX;
    float splitY = 380.f;

    // --- PANELLER ---
    sf::RectangleShape redPanel({leftWidth, splitY});
    redPanel.setPosition({gameStartX, 0.f}); 
    redPanel.setFillColor(sf::Color(20, 10, 10)); 

    sf::Sprite statSprite(statTexture);
    statSprite.setPosition({gameStartX + leftWidth, 0.f}); 
    float statScaleX = rightWidth / static_cast<float>(statTexture.getSize().x);
    float statScaleY = splitY / static_cast<float>(statTexture.getSize().y);
    statSprite.setScale({statScaleX, statScaleY});

    sf::Sprite dialogSprite(dialogTexture);
    dialogSprite.setPosition({gameStartX, splitY});
    float diaScaleX = leftWidth / static_cast<float>(dialogTexture.getSize().x);      
    float diaScaleY = (540.f - splitY) / static_cast<float>(dialogTexture.getSize().y); 
    dialogSprite.setScale({diaScaleX, diaScaleY});

    // --- ENVANTER ARAYUZU (OVERLAY) ---
    sf::RectangleShape inventoryBg({leftWidth, splitY}); // POV boyutuyla ayni
    inventoryBg.setPosition({gameStartX, 0.f});
    inventoryBg.setFillColor(sf::Color(0, 0, 0, 210)); // Saydam Siyah

    sf::Text inventoryText(font);
    inventoryText.setCharacterSize(20);
    inventoryText.setFillColor(sf::Color(255, 215, 0)); // Altin sarisi
    inventoryText.setPosition({gameStartX + 50.f, 50.f});

    std::vector<EnemyTarget> enemies; 
    
    // --- YAZILAR ---
    sf::Text statText(font);
    statText.setCharacterSize(16);
    statText.setFillColor(BORDEAUX_COLOR);
    statText.setPosition({statSprite.getPosition().x + 35.f, statSprite.getPosition().y + 40.f});
    updateStatText(statText, hero);

    float textPadX = 47.f; 
    float textPadY = 30.f; 

    sf::Text npcText(font);
    npcText.setString("Gatekeeper:\n'The bag opens anytime now.'");
    npcText.setCharacterSize(16);
    npcText.setFillColor(BORDEAUX_COLOR); 
    npcText.setPosition({dialogSprite.getPosition().x + textPadX, dialogSprite.getPosition().y + textPadY}); 

    std::vector<DialogueOption> options;
    std::vector<Button> buttons;
    float colWidth = rightWidth / 2.f;       
    float areaHeight = 540.f - splitY;       
    float yellowH = areaHeight / 4.f;        
    float actionStartX = gameStartX + leftWidth;
    float actionStartY = splitY;

    for (int i = 0; i < 4; i++) {
        buttons.emplace_back(
            "BTN_" + std::to_string(i), 
            "", 
            buttonTexture,   
            font,            
            sf::Vector2f(actionStartX, actionStartY + (i * yellowH)),
            sf::Vector2f(colWidth, yellowH)
        );
    }
    updateButtonLabels(buttons, currentState);

    float purpleH = areaHeight / 2.f;        
    buttons.emplace_back("MAP", "", mapTexture, font, sf::Vector2f(actionStartX + colWidth, actionStartY), sf::Vector2f(colWidth, purpleH));
    buttons.emplace_back("INV", "", inventoryTexture, font, sf::Vector2f(actionStartX + colWidth, actionStartY + purpleH), sf::Vector2f(colWidth, purpleH));

    while (window.isOpen()) {
        
        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);
        bool isMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape) window.close();
                
                // TEST: Durum Degistirme (C ve E)
                if (keyPressed->code == sf::Keyboard::Key::C) {
                    currentState = GameState::COMBAT;
                    updateButtonLabels(buttons, currentState);
                    updateDialogue(npcText, "COMBAT STARTED! (Inventory still works)");
                }
                if (keyPressed->code == sf::Keyboard::Key::E) {
                    currentState = GameState::EXPLORING;
                    updateButtonLabels(buttons, currentState);
                    updateDialogue(npcText, "Exploring mode.");
                }
            }
            
            if (const auto* mousePress = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePress->button == sf::Mouse::Button::Left) {
                    sf::Vector2f clickPosF = window.mapPixelToCoords(mousePress->position); 
                    
                    // Dusman Tiklama (Sadece Envanter Kapaliysa)
                    if (!isInventoryOpen) {
                        for (auto& enemy : enemies) { 
                            if (enemy.isClicked(clickPosF)) updateDialogue(npcText, "Target: " + enemy.id);
                        }
                    }

                    for (auto& btn : buttons) { 
                        if (btn.isClicked(clickPosF)) {
                            
                            // --- 1. INV BUTONU (Her zaman calisir) ---
                            if (btn.id == "INV") {
                                isInventoryOpen = !isInventoryOpen; // Aciksa kapat, kapaliysa ac (Toggle)
                                
                                if (isInventoryOpen) {
                                    updateInventoryText(inventoryText, hero);
                                    updateDialogue(npcText, "Inventory Opened.");
                                } else {
                                    updateDialogue(npcText, "Inventory Closed.");
                                }
                            }
                            
                            // --- 2. MAP BUTONU ---
                            else if (btn.id == "MAP" && !isInventoryOpen) {
                                updateDialogue(npcText, "Map clicked.");
                            }

                            // --- 3. AKSIYON BUTONLARI (Sadece Envanter Kapaliysa) ---
                            else if (!isInventoryOpen) {
                                if (currentState == GameState::EXPLORING) {
                                    if (btn.id == "BTN_0") updateDialogue(npcText, "Moving NORTH...");
                                    else if (btn.id == "BTN_1") updateDialogue(npcText, "Moving WEST...");
                                    else if (btn.id == "BTN_2") updateDialogue(npcText, "Moving EAST...");
                                    else if (btn.id == "BTN_3") updateDialogue(npcText, "Moving SOUTH...");
                                }
                                else if (currentState == GameState::COMBAT) {
                                    if (btn.id == "BTN_0") updateDialogue(npcText, "ATTACK!");
                                    else if (btn.id == "BTN_1") updateDialogue(npcText, "DEFEND!");
                                    else if (btn.id == "BTN_2") updateDialogue(npcText, "ITEM Used!");
                                    else if (btn.id == "BTN_3") updateDialogue(npcText, "RUN!");
                                }
                            }
                        } 
                    }
                }
            }
        }

        // --- UPDATE ---
        for (auto& opt : options) {
            if (opt.isHovered(mousePos)) { 
                opt.text.setFillColor(sf::Color::Red); 
                opt.text.setStyle(sf::Text::Underlined); 
            } else { 
                opt.text.setFillColor(BORDEAUX_COLOR); 
                opt.text.setStyle(sf::Text::Regular); 
            }
        }
        
        // Envanter acik degilse butonlari guncelle
        for (auto& btn : buttons) btn.update(mousePos, isMousePressed);


        // --- CIZIM (DRAW) ---
        window.clear(sf::Color::Black); 
        window.setView(gameView);

        // 1. ZEMIN ve OYUN DUNYASI
        window.draw(redPanel);
        for (const auto& enemy : enemies) window.draw(enemy.shape);

        // 2. OVERLAY (UST KATMAN)
        if (isInventoryOpen) {
            window.draw(inventoryBg);   // Yari saydam perde
            window.draw(inventoryText); // Liste
        }

        // 3. ARAYUZ (GUI)
        window.draw(statSprite);
        window.draw(statText); 
        window.draw(dialogSprite); 
        for (auto& btn : buttons) btn.draw(window);
        window.draw(npcText);
        for (const auto& opt : options) window.draw(opt.text);

        window.display();
    }

    return 0;
}