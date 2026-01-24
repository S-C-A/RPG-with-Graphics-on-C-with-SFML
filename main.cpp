#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

// --- ENTEGRASYON: GAME ENGINE ---
// Game.h dosyasini dahil ediyoruz. Bu dosya Player.h'i da iceriyor.
#include "Game.h" 

// --- OYUN DURUMLARI ---
enum class GameState {
    EXPLORING,
    COMBAT
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

// --- DUSMAN YAPISI ---
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

// --- YENI: ODA GUNCELLEME YARDIMCISI ---
// Oda degistiginde POV ekranindaki kareleri yenileyecek.
void updateEnemiesInView(std::vector<EnemyTarget>& enemyShapes, Room* room, float startX, float startY) {
    enemyShapes.clear(); // Eski odadaki kareleri sil
    
    if (!room) return;

    // Odadaki canavar ID'lerini gez
    float offsetX = 0;
    for (int mID : room->monsterID) {
        // Her canavar icin bir kare olustur
        EnemyTarget target("Monster_" + std::to_string(mID), startX + offsetX, startY);
        enemyShapes.push_back(target);
        offsetX += 120.f; // Bir sonraki kareyi saga kaydir
    }
}

// --- GORSEL SECENEK YAPISI (ISIM DEGISTI!) ---
// 'DialogueOption' ismi backend dosyalarinda da oldugu icin cakisma yapiyordu.
// Ismini 'VisualOption' yaptik. Sorun cozuldu.
struct VisualOption {
    sf::Text text;
    std::string id;
    VisualOption(sf::Font& font, std::string _text, std::string _id, float x, float y) : text(font) {
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
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "RPG - Fixed Integration", sf::State::Fullscreen);
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(true);
    sf::View gameView(sf::FloatRect({0.f, 0.f}, {960.f, 540.f}));
    window.setView(gameView);

    GameState currentState = GameState::EXPLORING;
    bool isInventoryOpen = false; 

    // --- GAME ENGINE BASLATIYORUZ ---
    Game game; 

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

    // --- ENVANTER OVERLAY ---
    sf::RectangleShape inventoryBg({leftWidth, splitY}); 
    inventoryBg.setPosition({gameStartX, 0.f});
    inventoryBg.setFillColor(sf::Color(0, 0, 0, 210)); 

    sf::Text inventoryText(font);
    inventoryText.setCharacterSize(20);
    inventoryText.setFillColor(sf::Color(255, 215, 0)); 
    inventoryText.setPosition({gameStartX + 50.f, 50.f});

    // --- POV ELEMENTS ---
    std::vector<EnemyTarget> enemies; 
    float centerX = gameStartX + (leftWidth / 2.f); 
    float centerY = splitY / 2.f;

    // ILK ODA GUNCELLEMESI
    // Baslangic odasina bakip (Game::lookAtRoom) canavarlari ciziyoruz.
    updateEnemiesInView(enemies, game.getCurrentRoom(), centerX - 50.f, centerY - 50.f);
    
    // --- TEXTS ---
    sf::Text statText(font);
    statText.setCharacterSize(16);
    statText.setFillColor(BORDEAUX_COLOR);
    statText.setPosition({statSprite.getPosition().x + 35.f, statSprite.getPosition().y + 40.f});
    
    // Game nesnesinden player'i aliyoruz (Artik yerel hero yok!)
    updateStatText(statText, game.getPlayer());

    float textPadX = 47.f; 
    float textPadY = 30.f; 

    sf::Text npcText(font);
    // Baslangic odasinin tarifini yaz (Game.h'dan geliyor)
    npcText.setString("System:\n" + game.lookAtRoom());
    npcText.setCharacterSize(16);
    npcText.setFillColor(BORDEAUX_COLOR); 
    npcText.setPosition({dialogSprite.getPosition().x + textPadX, dialogSprite.getPosition().y + textPadY}); 

    // --- BUTTONS & OPTIONS ---
    std::vector<VisualOption> options; // DIKKAT: VisualOption oldu
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

    // --- MAIN LOOP ---
    while (window.isOpen()) {
        
        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);
        bool isMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape) window.close();
                
                // TEST CHEAT: G
                if (keyPressed->code == sf::Keyboard::Key::G) {
                    game.addGoldCheat(50);
                    updateStatText(statText, game.getPlayer());
                    updateDialogue(npcText, "Cheat: +50 Gold added.");
                }
            }
            
            if (const auto* mousePress = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePress->button == sf::Mouse::Button::Left) {
                    sf::Vector2f clickPosF = window.mapPixelToCoords(mousePress->position); 
                    
                    // Dusman Tiklama
                    if (!isInventoryOpen) {
                        for (auto& enemy : enemies) { 
                            if (enemy.isClicked(clickPosF)) updateDialogue(npcText, "Target: " + enemy.id);
                        }
                    }

                    for (auto& btn : buttons) { 
                        if (btn.isClicked(clickPosF)) {
                            
                            // INV
                            if (btn.id == "INV") {
                                isInventoryOpen = !isInventoryOpen; 
                                if (isInventoryOpen) {
                                    updateInventoryText(inventoryText, game.getPlayer());
                                    updateDialogue(npcText, "Inventory Opened.");
                                } else {
                                    updateDialogue(npcText, "Inventory Closed.");
                                }
                            }
                            // MAP
                            else if (btn.id == "MAP" && !isInventoryOpen) {
                                updateDialogue(npcText, "Map clicked.");
                            }
                            // ACTIONS
                            else if (!isInventoryOpen) {
                                if (currentState == GameState::EXPLORING) {
                                    Room* current = game.getCurrentRoom();
                                    std::string moveMsg = "";
                                    bool moved = false;

                                    // YON TUSLARI: Game Engine'e emri gonder
                                    if (btn.id == "BTN_0") { moveMsg = game.attemptMove(current->n); moved = true; }
                                    else if (btn.id == "BTN_1") { moveMsg = game.attemptMove(current->w); moved = true; }
                                    else if (btn.id == "BTN_2") { moveMsg = game.attemptMove(current->e); moved = true; }
                                    else if (btn.id == "BTN_3") { moveMsg = game.attemptMove(current->s); moved = true; }

                                    if (moved) {
                                        // Hareket sonrasi yaziyi ve POV'u guncelle
                                        updateDialogue(npcText, moveMsg);
                                        updateEnemiesInView(enemies, game.getCurrentRoom(), centerX - 50.f, centerY - 50.f);
                                    }
                                }
                                else if (currentState == GameState::COMBAT) {
                                    // Buraya savas kodlari gelecek
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
        
        for (auto& btn : buttons) btn.update(mousePos, isMousePressed);

        // --- DRAW ---
        window.clear(sf::Color::Black); 
        window.setView(gameView);

        window.draw(redPanel);
        for (const auto& enemy : enemies) window.draw(enemy.shape);

        if (isInventoryOpen) {
            window.draw(inventoryBg);   
            window.draw(inventoryText); 
        }

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