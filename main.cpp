#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

// --- ENTEGRASYON: GAME ENGINE ---
#include "Game.h" 

enum class GameState {
    EXPLORING,
    COMBAT
};

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

// --- ENVANTER CIZME ---
void drawInventory(sf::RenderWindow& window, sf::Font& font, Player& player, float startX, float startY, sf::Vector2f mousePos) {
    const auto& inv = player.getInventory();
    
    sf::Text title(font);
    title.setString("--- BACKPACK ---");
    title.setCharacterSize(20);
    title.setFillColor(sf::Color(255, 215, 0)); 
    title.setPosition({startX, startY});
    window.draw(title);

    float currentY = startY + 40.f; 
    
    for (int i = 0; i < 10; ++i) {
        sf::Text slotText(font);
        slotText.setCharacterSize(18);
        slotText.setPosition({startX, currentY});

        if (i < inv.size()) {
            slotText.setString("[" + std::to_string(i + 1) + "] " + inv[i]->getName());
            if (slotText.getGlobalBounds().contains(mousePos)) {
                slotText.setFillColor(sf::Color(255, 100, 100)); // Hover
            } else {
                slotText.setFillColor(sf::Color::Red); 
            }
        } else {
            slotText.setString("[" + std::to_string(i + 1) + "] - Empty -");
            slotText.setFillColor(sf::Color(100, 100, 100)); 
        }

        window.draw(slotText);
        currentY += 25.f; 
    }
}

// --- DAKTILO EFEKTI (BEKLEMELI) ---
struct Typewriter {
    std::string fullText;     
    std::string currentText;  
    sf::Clock charClock;      
    sf::Clock waitClock;      
    
    size_t charIndex = 0;     
    float speed = 0.025f;     
    bool isFinished = true;   
    float waitTime = 1.5f; // Yazi bitince 1.5 sn bekle

    void start(std::string message) {
        fullText = "System:\n" + message; 
        currentText = "";                 
        charIndex = 0;                    
        isFinished = false;               
        charClock.restart();
    }

    void update(sf::Text& textObj) {
        if (charIndex < fullText.size()) {
            if (charClock.getElapsedTime().asSeconds() > speed) {
                currentText += fullText[charIndex]; 
                charIndex++;
                charClock.restart(); 
                
                if (charIndex >= fullText.size()) {
                    isFinished = true; 
                    waitClock.restart(); 
                }
            }
        } 
    }
    
    void finishInstant() {
        currentText = fullText;
        charIndex = fullText.size();
        isFinished = true;
        waitClock.restart(); 
    }

    bool isBusy() {
        if (charIndex < fullText.size()) return true;
        if (waitClock.getElapsedTime().asSeconds() < waitTime) return true;
        return false;
    }

    std::string getCurrentText() { return currentText; }
};

// --- YERDEKI ESYA YAPISI (LOOT - SFML 3.0 UYUMLU) ---
struct ItemTarget {
    sf::RectangleShape shape;
    std::string name;
    
    ItemTarget(std::string _name, float x, float y) {
        name = _name;
        shape.setSize({40.f, 40.f}); 
        shape.setPosition({x, y});
        shape.setFillColor(sf::Color::Yellow); 
        shape.setOutlineThickness(2.f);
        shape.setOutlineColor(sf::Color(200, 200, 0));
        
        // SFML 3.0: Derece donusumu
        shape.setRotation(sf::degrees(45.f)); 
    }

    void update(sf::Vector2f mousePos) {
        if (shape.getGlobalBounds().contains(mousePos)) {
            // SFML 3.0: Vektor kullanimi
            shape.setScale({1.1f, 1.1f});
            shape.setFillColor(sf::Color::White); 
        } else {
            shape.setScale({1.0f, 1.0f});
            shape.setFillColor(sf::Color::Yellow);
        }
    }

    bool isClicked(sf::Vector2f mousePos) {
        return shape.getGlobalBounds().contains(mousePos);
    }
};

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

void updateEnemiesInView(std::vector<EnemyTarget>& enemyShapes, Room* room, float startX, float startY) {
    enemyShapes.clear(); 
    if (!room) return;
    float offsetX = 0;
    for (int mID : room->monsterID) {
        EnemyTarget target("Monster_" + std::to_string(mID), startX + offsetX, startY);
        enemyShapes.push_back(target);
        offsetX += 120.f; 
    }
}

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

struct Button {
    sf::Sprite sprite; 
    sf::Text label;    
    std::string id;
    sf::Vector2f baseScale; 
    sf::Vector2f m_targetSize; 

    Button(std::string _id, std::string _labelText, const sf::Texture& texture, sf::Font& font, sf::Vector2f position, sf::Vector2f targetSize) 
        : sprite(texture), label(font), m_targetSize(targetSize)
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

    void setStyle(bool active, const sf::Texture& tex) {
        sprite.setTexture(tex); 
        sf::Vector2u texSize = tex.getSize();
        sprite.setOrigin({static_cast<float>(texSize.x) / 2.f, static_cast<float>(texSize.y) / 2.f});
        float scaleX = m_targetSize.x / static_cast<float>(texSize.x);
        float scaleY = m_targetSize.y / static_cast<float>(texSize.y);
        baseScale = {scaleX, scaleY};
        sprite.setScale(baseScale);
        if (active) label.setFillColor(BORDEAUX_COLOR);
        else label.setFillColor(sf::Color(100, 100, 100)); 
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

void updateButtonStates(std::vector<Button>& buttons, GameState state, Room* room, 
                        const sf::Texture& normalTex, const sf::Texture& greyTex) {
    if (state == GameState::EXPLORING) {
        buttons[0].setLabelText("NORTH");
        buttons[1].setLabelText("WEST");
        buttons[2].setLabelText("EAST");
        buttons[3].setLabelText("SOUTH");
        if (room) {
            if (room->n == -1) buttons[0].setStyle(false, greyTex); else buttons[0].setStyle(true, normalTex);
            if (room->w == -1) buttons[1].setStyle(false, greyTex); else buttons[1].setStyle(true, normalTex);
            if (room->e == -1) buttons[2].setStyle(false, greyTex); else buttons[2].setStyle(true, normalTex);
            if (room->s == -1) buttons[3].setStyle(false, greyTex); else buttons[3].setStyle(true, normalTex);
        }
    } 
    else if (state == GameState::COMBAT) {
        for(auto& btn : buttons) btn.setStyle(true, normalTex);
        buttons[0].setLabelText("ATK");
        buttons[1].setLabelText("DEF");
        buttons[2].setLabelText("ITEM");
        buttons[3].setLabelText("RUN");
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "RPG - Ultimate Loot Edition", sf::State::Fullscreen);
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(true);
    sf::View gameView(sf::FloatRect({0.f, 0.f}, {960.f, 540.f}));
    window.setView(gameView);

    GameState currentState = GameState::EXPLORING;
    bool isInventoryOpen = false; 

    Game game; 
    Typewriter typer; 

    sf::Font font;
    if (!font.openFromFile("font.ttf")) return -1;

    // --- TEXTURES ---
    sf::Texture dialogTexture; if (!dialogTexture.loadFromFile("textures/Textbox[Final].png")) return -1;
    sf::Texture statTexture; if (!statTexture.loadFromFile("textures/Statbox[Final].png")) return -1;
    sf::Texture buttonTexture; if (!buttonTexture.loadFromFile("textures/Button[Final].png")) return -1;
    sf::Texture inventoryTexture; if (!inventoryTexture.loadFromFile("textures/Inventory[Final].png")) return -1;
    sf::Texture mapTexture; if (!mapTexture.loadFromFile("textures/Map[Final].png")) { mapTexture = inventoryTexture; }
    sf::Texture buttonGreyTexture; if (!buttonGreyTexture.loadFromFile("textures/Button[Grey].png")) return -1;

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

    sf::RectangleShape inventoryBg({leftWidth, splitY}); 
    inventoryBg.setPosition({gameStartX, 0.f});
    inventoryBg.setFillColor(sf::Color(0, 0, 0, 210)); 

    std::vector<EnemyTarget> enemies; 
    std::vector<ItemTarget> groundItems; // Yerdeki esyalar (Loot)
    float centerX = gameStartX + (leftWidth / 2.f); 
    float centerY = splitY / 2.f;

    // Baslangic Update'i
    updateEnemiesInView(enemies, game.getCurrentRoom(), centerX - 50.f, centerY - 50.f);
    groundItems.clear();
    if (game.getCurrentRoom() && game.getCurrentRoom()->itemID != -1) {
        groundItems.emplace_back("Loot", centerX, centerY + 50.f);
    }
    
    sf::Text statText(font);
    statText.setCharacterSize(16);
    statText.setFillColor(BORDEAUX_COLOR);
    statText.setPosition({statSprite.getPosition().x + 35.f, statSprite.getPosition().y + 40.f});
    updateStatText(statText, game.getPlayer());

    float textPadX = 47.f; 
    float textPadY = 30.f; 

    sf::Text npcText(font);
    npcText.setCharacterSize(16);
    npcText.setFillColor(BORDEAUX_COLOR); 
    npcText.setPosition({dialogSprite.getPosition().x + textPadX, dialogSprite.getPosition().y + textPadY}); 
    
    typer.start(game.lookAtRoom());

    std::vector<VisualOption> options;
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
    updateButtonStates(buttons, currentState, game.getCurrentRoom(), buttonTexture, buttonGreyTexture);

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
            }
            
            if (const auto* mousePress = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePress->button == sf::Mouse::Button::Left) {
                    sf::Vector2f clickPosF = window.mapPixelToCoords(mousePress->position); 
                    
                    // --- 1. DAKTILO HIZLI GECME ---
                    if (typer.isBusy() && !typer.isFinished) {
                        typer.finishInstant(); 
                        npcText.setString(typer.getCurrentText());
                        continue; 
                    }

                    // --- 2. ENVANTER TIKLAMA ---
                    if (isInventoryOpen) {
                        float listStartX = gameStartX + 50.f;
                        float listStartY = 50.f + 40.f;
                        float lineHeight = 25.f;

                        if (clickPosF.x >= listStartX && clickPosF.x <= listStartX + 300.f) {
                            int clickedIndex = (int)((clickPosF.y - listStartY) / lineHeight);
                            if (clickedIndex >= 0 && clickedIndex < 10) {
                                std::string result = game.playerUseItem(clickedIndex);
                                if (!result.empty()) {
                                    typer.start(result); 
                                    updateStatText(statText, game.getPlayer()); 
                                }
                            }
                        }
                    } 
                    else {
                        // --- 3. DUSMAN TIKLAMA ---
                        for (auto& enemy : enemies) { 
                            if (enemy.isClicked(clickPosF)) typer.start("Target: " + enemy.id);
                        }
                        
                        // --- 4. YERDEKI ESYAYI (LOOT) TOPLAMA (YENI) ---
                        for (auto& item : groundItems) {
                            if (item.isClicked(clickPosF)) {
                                std::string msg = game.tryPickupItem();
                                typer.start(msg);
                                // Eger aldiysak ekrandan sil
                                if (msg.find("Picked up") != std::string::npos) {
                                    groundItems.clear();
                                    updateStatText(statText, game.getPlayer());
                                }
                            }
                        }
                    }

                    // --- 5. BUTON TIKLAMA ---
                    for (auto& btn : buttons) { 
                        if (btn.isClicked(clickPosF)) {
                            // INV
                            if (btn.id == "INV") {
                                isInventoryOpen = !isInventoryOpen; 
                                if (isInventoryOpen) typer.start("Inventory Opened.");
                                else typer.start(game.lookAtRoom()); // Eski yerine dondur
                            }
                            // MAP
                            else if (btn.id == "MAP" && !isInventoryOpen) {
                                typer.start("Map system active.");
                            }
                            // ACTIONS
                            else if (!isInventoryOpen) {
                                if (currentState == GameState::EXPLORING) {
                                    Room* current = game.getCurrentRoom();
                                    std::string moveMsg = "";
                                    
                                    if (btn.id == "BTN_0") moveMsg = game.attemptMove(current->n);
                                    else if (btn.id == "BTN_1") moveMsg = game.attemptMove(current->w);
                                    else if (btn.id == "BTN_2") moveMsg = game.attemptMove(current->e);
                                    else if (btn.id == "BTN_3") moveMsg = game.attemptMove(current->s);

                                    typer.start(moveMsg);
                                    updateEnemiesInView(enemies, game.getCurrentRoom(), centerX - 50.f, centerY - 50.f);
                                    // Hareket edince yerdeki esyalari da guncelle (Varsa ciz, yoksa sil)
                                    groundItems.clear();
                                    if (game.getCurrentRoom() && game.getCurrentRoom()->itemID != -1) {
                                        groundItems.emplace_back("Loot", centerX, centerY + 50.f);
                                    }
                                    
                                    updateButtonStates(buttons, currentState, game.getCurrentRoom(), buttonTexture, buttonGreyTexture);
                                }
                                else if (currentState == GameState::COMBAT) {
                                    if (btn.id == "BTN_0") typer.start("ATTACK!");
                                    else if (btn.id == "BTN_1") typer.start("DEFEND!");
                                    else if (btn.id == "BTN_2") typer.start("ITEM Used!");
                                    else if (btn.id == "BTN_3") typer.start("RUN!");
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
        for (auto& item : groundItems) item.update(mousePos); // Loot update
        
        // --- TEXT GUNCELLEME VE HOVER MANTIGI ---
        
        typer.update(npcText);

        bool isHoveringItem = false;
        
        if (isInventoryOpen && !typer.isBusy()) { 
            float listStartX = gameStartX + 50.f;
            float listStartY = 50.f + 40.f;
            float lineHeight = 25.f;

            if (mousePos.x >= listStartX && mousePos.x <= listStartX + 300.f) {
                int hoverIndex = (int)((mousePos.y - listStartY) / lineHeight);
                
                if (hoverIndex >= 0 && hoverIndex < 10) {
                    std::string desc = game.getItemDesc(hoverIndex);
                    if (!desc.empty()) {
                        npcText.setString("Description:\n" + desc); 
                        isHoveringItem = true;
                    }
                }
            }
        }

        if (!isHoveringItem) {
            npcText.setString(typer.getCurrentText());
        }

        // --- DRAW ---
        window.clear(sf::Color::Black); 
        window.setView(gameView);

        window.draw(redPanel);
        for (const auto& enemy : enemies) window.draw(enemy.shape);
        for (const auto& item : groundItems) window.draw(item.shape); // Loot ciz

        if (isInventoryOpen) {
            window.draw(inventoryBg);   
            drawInventory(window, font, game.getPlayer(), gameStartX + 50.f, 50.f, mousePos);
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