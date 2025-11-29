#pragma once
#include "monster.h"
#include "player.h"  // Player'a müdahale edeceği için şart
#include "condition.h"  // Durum etkilerini (Mark, Bleed) kullanacak
#include <iostream>
#include <cstdlib>   // rand() fonksiyonu için

// --- BANDIT SLASHER ---
class BanditSlasher : public Monster {
public:
    // Constructor: İsim ve Statları burada sabitliyoruz
    // HP:24, Atk:5, Def:3, Gold:15
    BanditSlasher(int id) 
        : Monster(id, "Bandit Slasher", "Tehlikeli bir haydut.", 24, 5, 3, 15) 
    {}

    // Klonlama (Zorunlu)
    Monster* clone() const override {
        return new BanditSlasher(*this);
    }

    // --- SALDIRI MANTIGI / DAVRANIS ---
    void makeMove(Player* target) override {
        
        // 1. ÖZEL DURUM: Hedef İşaretli mi (MARK)?
        // İşaretliyse kesinlikle "Bitirici Vuruş" yapar.
        if (target->hasStatus(MARK)) {
            std::cout << ">> Bandit Slasher isaretli hedefini goruyor ve cilginca saldiriyor!" << std::endl;
            
            // Etki: Normal Hasar + Güçlü Kanama
            target->hurt(atk);
            target->addStatus(StatusEffect(BLEED, 3, 3)); // Güç:3, Süre:3
            
            std::cout << "KRITIK! " << atk << " hasar aldin ve derin bir yara aldin!" << std::endl;
        }
        else {
            // 2. NORMAL DURUM: Rastgele Seçim
            int choice = rand() % 3; // 0, 1 veya 2 döner

            switch (choice) {
            case 0: // NORMAL SALDIRI
                std::cout << ">> Bandit Slasher kilicini savurdu." << std::endl;
                target->hurt(atk); 
                std::cout << atk << " hasar aldin." << std::endl;
                break;

            case 1: // KANATAN SALDIRI (Az Hasar + Kanama)
                std::cout << ">> Bandit Slasher sinsi bir hamle yapti!" << std::endl;
                target->hurt(2); // Sabit 2 hasar
                target->addStatus(StatusEffect(BLEED, 2, 3)); // Güç:2, Süre:3
                std::cout << "2 hasar aldin ve hafifce kaniyorsun." << std::endl;
                break;

            case 2: // İŞARETLEME (Hasar Yok + Mark)
                std::cout << ">> Bandit Slasher gozlerini sana dikti..." << std::endl;
                // Hasar vermiyor, sadece MARK ekliyor
                target->addStatus(StatusEffect(MARK, 0, 3)); 
                std::cout << "ISARETLENDIN! (Bir sonraki saldiriya dikkat et)" << std::endl;
                break;
            }
        }
    }
};

// İleride başka canavarlar eklemek istersen (Goblin, Orc vb.)
// onların sınıflarını da buraya alt alta yazabilirsin.