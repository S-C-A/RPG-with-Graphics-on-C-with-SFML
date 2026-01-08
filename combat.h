#pragma once
#include <iostream>
#include <vector>
#include "player.h"
#include "monster.h"

using namespace std;

class CombatManager {
public:
    bool startBattle(Player* hero, std::vector<Monster*>& enemies) {
        
        for (auto n : enemies) cout << n->getName() << " blocks your path!";    
        cout << "\n";
        
        bool isBattleOver = false;
        bool playerWon = false;

        while (!isBattleOver) {     
            
            // --- (Player Turn) ---
            
            bool playerTurn = false;

            while (!playerTurn)
            {
                /* code */
            }
            
            // 1. Durum Gösterimi:
            // Oyuncunun canını ve karşısındaki düşmanların durumunu (Canı, Adı) listele.
            // Örn: [1] Goblin (HP: 20)  [2] Haydut (HP: 50)

            // 2. Menü ve Seçim:
            // Oyuncuya ne yapmak istediğini sor: (1. Saldır, 2. İksir İç, 3. Kaç)
            
            // 3. Hedef Seçimi (Eğer Saldır seçildiyse):
            // Eğer birden fazla düşman varsa, hangisine vuracağını sor.
            // Seçilen düşmana hasar ver (enemy->takeDamage(hero->getAtk())).

            // 4. Ölüm Kontrolü (Düşmanlar için):
            // Vurulan düşman öldü mü? Öldüyse ekrana yaz.
            // Eğer TÜM düşmanlar öldüyse -> Döngüyü kır (break), playerWon = true yap.


            // --- B. DÜŞMAN TURU (Enemy Turn) ---
            
            // Hayatta kalan her düşman sırayla oyuncuya saldırmalı.
            // İpucu: for döngüsü ile enemies vektörünü gez.
            // Eğer düşman ölmediyse (!monster->isDead()) -> monster->makeMove(hero) çağır.

            // --- C. OYUNCU ÖLÜM KONTROLÜ ---
            // Düşmanlar vurduktan sonra oyuncu öldü mü? (hero->isAlive() veya hp <= 0)
            // Eğer öldüyse -> Döngüyü kır (break), playerWon = false yap.

        }

        // 3. SONUÇ
        // Döngü bittiğinde kazananı bildir.
        return playerWon;
    }
};