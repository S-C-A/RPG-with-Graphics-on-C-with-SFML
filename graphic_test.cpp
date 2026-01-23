#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

#include "Player.h" 

// --- RENK AYARI ---
const sf::Color BORDEAUX_COLOR = sf::Color(110, 0, 0); 

// --- YARDIMCI FONKSIYON: STATLARI EKRANA YAZ ---
// Burasi artik senin GERCEK Player sinifindan veri cekiyor!
void updateStatText(sf::Text& text, Player& player) {
    std::string content;
    
    // Baslik
    content += "   KEYBEARER\n";
    content += "     (Lvl " + std::to_string(player.getLvl()) + ")\n\n";
    
    // HP (Can)
    // Player.h icinde getHp() ve getMaxHp() metotlarin vardi, onlari kullaniyoruz.
    content += "HP:   " + std::to_string(player.getHp()) + "/" + std::to_string(player.getMaxHp()) + "\n\n";
    
    // ATK / DEF
    content += "ATK:  " + std::to_string(player.getAtk()) + "\n";
    content += "DEF:  " + std::to_string(player.getDef()) + "\n\n";
    
    // GOLD / EXP
    content += "GOLD: " + std::to_string(player.getGold()) + "\n";
    
    // Exp gosterimi (Max level degilse goster)
    // getExp() metodu vardi.
    content += "EXP:  " + std::to_string(player.getExp()) + "\n\n";

    // Weapon
    content += "Weapon\n";
    content += "[" + player.getWeaponName() + "]\n\n";

    // Armor
    content += "Armor\n";
    content += "[" + player.getArmorName() + "]";


    text.setString(content);
}

// --- YARDIMCI FONKSIYON: DIYALOG KUTUSUNU GUNCELLE ---
// Konsola cout yapmak yerine artik burayi kullanacagiz.
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

// --- DIYALOG SECENEK YAPISI ---
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

// --- BUTON YAPISI (JUICE EFEKTI AKTIF) ---
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
            if (isMousePressed) {
                sprite.setScale({baseScale.x * 0.85f, baseScale.y * 0.85f});
            } else {
                sprite.setScale({baseScale.x * 0.95f, baseScale.y * 0.95f});
            }
        } else {
            sprite.setScale(baseScale);
        }
    }

    bool isClicked(sf::Vector2f mousePos) {
        return sprite.getGlobalBounds().contains(mousePos);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
        if (label.getString().getSize() > 0) {
            window.draw(label);
        }
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "RPG - Backend Connected", sf::State::Fullscreen);
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(true);
    sf::View gameView(sf::FloatRect({0.f, 0.f}, {960.f, 540.f}));
    window.setView(gameView);

    // --- 1. GERCEK OYUNCUYU OLUSTUR ---
    // Player sinifinin constructor'inda lvl=1, gold=17 gibi baslangic degerleri vardi.
    // Artik o degerlerle baslayacak.
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

    std::vector<EnemyTarget> enemies;
    float centerX = gameStartX + (leftWidth / 2.f);
    float centerY = splitY / 2.f;
    enemies.emplace_back("GOBLIN_WARRIOR", centerX - 110.f, centerY - 50.f);
    enemies.emplace_back("GOBLIN_MAGE", centerX + 10.f, centerY - 50.f);

    // --- YAZILAR ---
    
    // STAT TEXT (Dinamik)
    sf::Text statText(font);
    statText.setCharacterSize(16);
    statText.setFillColor(BORDEAUX_COLOR);
    statText.setPosition({statSprite.getPosition().x + 35.f, statSprite.getPosition().y + 40.f});
    
    // ILK GUNCELLEME: Oyun basinda statlari yazdir
    updateStatText(statText, hero);


    float textPadX = 47.f; 
    float textPadY = 30.f; 

    sf::Text npcText(font);
    npcText.setString("Gatekeeper:\n'Your stats are real now.'");
    npcText.setCharacterSize(16);
    npcText.setFillColor(BORDEAUX_COLOR); 
    npcText.setPosition({dialogSprite.getPosition().x + textPadX, dialogSprite.getPosition().y + textPadY}); 

    std::vector<DialogueOption> options;
    options.emplace_back(font, "> Check Gold.", "OPT_YES", dialogSprite.getPosition().x + textPadX, dialogSprite.getPosition().y + textPadY + 45.f);
    options.emplace_back(font, "> Do nothing.", "OPT_NO", dialogSprite.getPosition().x + textPadX, dialogSprite.getPosition().y + textPadY + 70.f);

    std::vector<Button> buttons;
    float colWidth = rightWidth / 2.f;       
    float areaHeight = 540.f - splitY;       
    float yellowH = areaHeight / 4.f;        
    float actionStartX = gameStartX + leftWidth;
    float actionStartY = splitY;

    std::vector<std::string> actionLabels = {"ATK", "DEF", "ITEM", "RUN"};
    for (int i = 0; i < 4; i++) {
        buttons.emplace_back(
            "BTN_" + std::to_string(i), 
            actionLabels[i], 
            buttonTexture,   
            font,            
            sf::Vector2f(actionStartX, actionStartY + (i * yellowH)),
            sf::Vector2f(colWidth, yellowH)
        );
    }

    float purpleH = areaHeight / 2.f;        
    buttons.emplace_back("MAP", "", mapTexture, font, sf::Vector2f(actionStartX + colWidth, actionStartY), sf::Vector2f(colWidth, purpleH));
    buttons.emplace_back("INV", "", inventoryTexture, font, sf::Vector2f(actionStartX + colWidth, actionStartY + purpleH), sf::Vector2f(colWidth, purpleH));

    // --- DONGU ---
    while (window.isOpen()) {
        
        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);
        bool isMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        while (const auto event = window.pollEvent()) {

            if (const auto* mousePress = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePress->button == sf::Mouse::Button::Left) {
                    sf::Vector2f clickPosF = window.mapPixelToCoords(mousePress->position); 
                    
                    // --- DUSMAN TIKLAMA ---
                    for (auto& enemy : enemies) { 
                        if (enemy.isClicked(clickPosF)) {
                            // Dusmana tiklayinca ismini diyaloga yaz
                            updateDialogue(npcText, "Target selected: " + enemy.id);
                        }
                    }

                    // --- DIYALOG SECENEKLERI ---
                    for (auto& opt : options) { 
                        if (opt.isHovered(clickPosF)) {
                             // Simdilik sadece loglayalim
                             std::cout << "DIYALOG: " << opt.id << std::endl; 
                        }
                    }
                    
                    // --- BUTON TIKLAMA (LOGIC BAGLANTISI BURADA) ---
                    for (auto& btn : buttons) { 
                        if (btn.isClicked(clickPosF)) {
                            
                            // 1. ATK (Saldiri) - BTN_0
                            if (btn.id == "BTN_0") {
                                // Ornek: XP kazandir ve diyaloga yaz
                                hero.addExp(10); 
                                updateDialogue(npcText, "'Strike true!'\n(You practice swinging. +10 XP)");
                            }
                            
                            // 2. DEF (Savunma) - BTN_1
                            else if (btn.id == "BTN_1") {
                                // Ornek: Cani biraz azalt (Test icin)
                                hero.hurt(5);
                                updateDialogue(npcText, "'Steady your stance.'\n(You blocked poorly. -5 HP)");
                            }
                            
                            // 3. ITEM (Esya) - BTN_2
                            else if (btn.id == "BTN_2") {
                                // Ornek: Cani doldur
                                hero.heal(15);
                                updateDialogue(npcText, "'A refreshing brew.'\n(Used Potion. +15 HP)");
                            }
                            
                            // 4. RUN (Kac) - BTN_3
                            else if (btn.id == "BTN_3") {
                                // Ornek: Para dusur
                                hero.goldChange(-5);
                                updateDialogue(npcText, "'Cowardice costs.'\n(You dropped some coins while running. -5 G)");
                            }

                            // 5. INVENTORY ve MAP butonlari
                            else if (btn.id == "INV") {
                                updateDialogue(npcText, "Opening Inventory backpack...");
                                // Ileride buraya envanter penceresi acma kodu gelecek
                            }
                            else if (btn.id == "MAP") {
                                updateDialogue(npcText, "Checking the map...");
                                // Ileride harita acma kodu gelecek
                            }

                            // ISLEM SONRASI STATLARI GUNCELLEMEYI UNUTMA!
                            updateStatText(statText, hero);
                        } 
                    }
                }
            }
            if (event->is<sf::Event::Closed>()) window.close();
            
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape) window.close();
                
                // TEST: 'G' tusuna basinca 50 Altin ekle (Senin sinifindaki goldChange metodunu kullaniyoruz)
                if (keyPressed->code == sf::Keyboard::Key::G) {
                    hero.goldChange(50);
                    updateStatText(statText, hero); // Ekrani guncelle
                    std::cout << "ALTIN KAZANILDI! Guncel: " << hero.getGold() << std::endl;
                }
                
                // TEST: 'H' tusuna basinca hasar al (Senin sinifindaki hurt metodu)
                if (keyPressed->code == sf::Keyboard::Key::H) {
                    hero.hurt(10);
                    updateStatText(statText, hero);
                    std::cout << "HASAR ALINDI! Guncel Can: " << hero.getHp() << std::endl;
                }
            }
            
            if (const auto* mousePress = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePress->button == sf::Mouse::Button::Left) {
                    sf::Vector2f clickPosF = window.mapPixelToCoords(mousePress->position); 
                    
                    for (auto& enemy : enemies) { if (enemy.isClicked(clickPosF)) std::cout << "DUSMAN: " << enemy.id << std::endl; }
                    for (auto& opt : options) { if (opt.isHovered(clickPosF)) std::cout << "DIYALOG: " << opt.id << std::endl; }
                    
                    for (auto& btn : buttons) { 
                        if (btn.isClicked(clickPosF)) std::cout << "BUTON: " << btn.id << std::endl; 
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
        for (auto& enemy : enemies) enemy.update(mousePos);

        for (auto& btn : buttons) {
            btn.update(mousePos, isMousePressed);
        }

        // --- CIZIM ---
        window.clear(sf::Color::Black); 
        window.setView(gameView);

        window.draw(redPanel);
        for (const auto& enemy : enemies) window.draw(enemy.shape);
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