#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "RPG UI - Text Test");
    window.setFramerateLimit(60);

    // --- 1. FONTLARI YUKLE ---
    // Eger "font.ttf" dosyasini bulamazsa hata verir ve kapanir.
    sf::Font font;
    if (!font.openFromFile("font.ttf")) {
        std::cerr << "HATA: font.ttf dosyasi bulunamadi!" << std::endl;
        // Font yoksa devam etmenin anlami yok, return ile kapatiyoruz.
        return -1;
    }

    // --- LAYOUT HESAPLARI (Senin Kodun) ---
    float topHeight = 425.f;    
    float bottomHeight = 175.f; 

    // 1. SOL PANEL (STATS)
    sf::RectangleShape leftPanel({200.f, topHeight});
    leftPanel.setFillColor(sf::Color(50, 50, 50)); 
    leftPanel.setPosition({0.f, 0.f}); 

    // 2. SAG ANA PANEL (POV)
    sf::RectangleShape mainPanel({600.f, topHeight});
    mainPanel.setFillColor(sf::Color(100, 0, 0)); 
    mainPanel.setPosition({200.f, 0.f}); 

    // 3. SAG ALT PANEL (DIALOG)
    sf::RectangleShape textPanel({600.f, bottomHeight});
    textPanel.setFillColor(sf::Color(0, 0, 100)); 
    textPanel.setPosition({200.f, topHeight}); 

    // 4. SOL ALT KUTULAR (EKIPMANLAR)
    sf::RectangleShape equipSlot1({100.f, bottomHeight}); 
    equipSlot1.setFillColor(sf::Color(100, 100, 0)); 
    equipSlot1.setPosition({0.f, topHeight}); 
    equipSlot1.setOutlineThickness(-2.f);
    equipSlot1.setOutlineColor(sf::Color::Black);

    sf::RectangleShape equipSlot2({100.f, bottomHeight}); 
    equipSlot2.setFillColor(sf::Color(0, 100, 0)); 
    equipSlot2.setPosition({100.f, topHeight}); 
    equipSlot2.setOutlineThickness(-2.f);
    equipSlot2.setOutlineColor(sf::Color::Black);

    // --- YENI: YAZILARI OLUSTUR ---

    // A) Sol Panel Yazilari (Stats)
    sf::Text statsText(font); // Fontu veriyoruz (SFML 3: constructor ile)
    statsText.setString("Name: Hero\n\nHP: 100/100\nGold: 50\nLvl: 1");
    statsText.setCharacterSize(18); // Yazi boyutu
    statsText.setFillColor(sf::Color::White);
    statsText.setPosition({10.f, 10.f}); // Sol panelin icinde, biraz bosluklu

    // B) Diyalog Yazisi (Sag Alt)
    sf::Text dialogText(font);
    dialogText.setString("You enter a dark dungeon.\nThe air smells of old dust.\nWhat will you do?");
    dialogText.setCharacterSize(20);
    dialogText.setFillColor(sf::Color::White);
    // Panelin baslangici (200, 425) oldugu icin biraz iceri (210, 435) koyuyoruz
    dialogText.setPosition({210.f, topHeight + 10.f}); 

    // C) Ekipman Yazilari (Sol Alt)
    sf::Text weaponLabel(font);
    weaponLabel.setString("WEAPON\n(Empty)");
    weaponLabel.setCharacterSize(14);
    weaponLabel.setFillColor(sf::Color::Black); // Sari zemin ustune siyah yazi
    weaponLabel.setPosition({10.f, topHeight + 10.f});

    sf::Text shieldLabel(font);
    shieldLabel.setString("SHIELD\n(Empty)");
    shieldLabel.setCharacterSize(14);
    shieldLabel.setFillColor(sf::Color::White); // Yesil zemin ustune beyaz yazi
    shieldLabel.setPosition({110.f, topHeight + 10.f});


    // --- OYUN DONGUSU ---
    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear(sf::Color::Black);

        // Once Kutulari Ciz
        window.draw(leftPanel);   
        window.draw(mainPanel);   
        window.draw(textPanel);   
        window.draw(equipSlot1);  
        window.draw(equipSlot2);  

        // Sonra Yazilari Ciz (Kutularin ustune gelsin diye en son ciziyoruz)
        window.draw(statsText);
        window.draw(dialogText);
        window.draw(weaponLabel);
        window.draw(shieldLabel);

        window.display();
    }

    return 0;
}