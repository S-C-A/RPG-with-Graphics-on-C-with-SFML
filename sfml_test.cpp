#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib> // rand() ve srand() için gerekli
#include <ctime>   // time() için gerekli (rastgeleliği zamana bağlamak için)

int main()
{
    // Rastgele sayı üretecinin tohumunu atıyoruz (Her çalışmada farklı olsun diye)
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    sf::RenderWindow window(sf::VideoMode({800, 600}), "Rastgele Secim");

    // --- 1. ADIM: ŞABLON KARELERİ OLUŞTUR (Döngüden Önce) ---
    // 3 farklı renkte kare hazırlayıp bir 'depoya' (vektöre) koyalım.
    std::vector<sf::RectangleShape> renkPaleti;

    // 1. Şablon: Kırmızı
    sf::RectangleShape k1({100.f, 100.f});
    k1.setFillColor(sf::Color::Red);
    renkPaleti.push_back(k1);

    // 2. Şablon: Mavi
    sf::RectangleShape k2({100.f, 100.f});
    k2.setFillColor(sf::Color::Blue);
    renkPaleti.push_back(k2);

    // 3. Şablon: Sarı
    sf::RectangleShape k3({100.f, 100.f});
    k3.setFillColor(sf::Color::Yellow);
    renkPaleti.push_back(k3);

    // --- Değişkenler ---
    sf::Clock saat;
    // Hangi sıradaki karenin hangi renkte olacağını tutan liste (5 tane)
    std::vector<int> secilenRenkler = {0, 0, 0, 0, 0}; 

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()) window.close();
            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
                if (key->code == sf::Keyboard::Key::Escape) window.close();
        }

        // --- ZAMANLAYICI ---
        // Her 1 saniyede bir rastgele seçimleri yenile
        // Eğer bunu yapmazsak renkler saniyede 60 kez değişir (göz yorar)
        if (saat.getElapsedTime().asSeconds() > 1.0f)
        {
            // 5 karenin her biri için rastgele bir şablon (0, 1 veya 2) seç
            for (int i = 0; i < 5; ++i)
            {
                // rand() % 3 -> Bize 0, 1 veya 2 sayısını verir
                secilenRenkler[i] = std::rand() % 3;
            }
            saat.restart();
        }

        window.clear();

        // --- 2. ADIM: ÇİZİM DÖNGÜSÜ ---
        // 5 tane kare çizeceğiz
        for (int i = 0; i < 5; ++i)
        {
            // A. Şablondan Kopyala
            // Hangi rengi seçmişiz? (secilenRenkler[i])
            // O rengi paletten al ve 'geciciKare'ye kopyala.
            sf::RectangleShape geciciKare = renkPaleti[secilenRenkler[i]];

            // B. Konumunu Ayarla (Üst üste binecek şekilde)
            // Kare boyutu 100, ama biz 40'ar 40'ar kaydırıyoruz -> Üst üste biner
            float x = 200.f + (i * 40.f);
            float y = 150.f + (i * 40.f);
            geciciKare.setPosition({x, y});

            // C. Çiz
            window.draw(geciciKare);
        }

        window.display();
    }

    return 0;
}