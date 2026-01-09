#pragma once
#include <iostream>
#include <vector>
#include <ctime>

// Gerekli tum dosyalari dahil ediyoruz
#include "player.h"
#include "ItemManager.h"
#include "enemyManager.h"
#include "room.h"
#include "combat.h" 

using namespace std;

class Game {
private:
    Player hero;
    ItemManager itemMgr;
    EnemyManager mobMgr;
    MapManager mapMgr;
    CombatManager combatMgr;

    Room* currentRoom;

    // --- ENVANTER VE EKIPMAN MENUSU ---
    void manageInventory() {
        bool inMenu = true;
        
        while (inMenu) {
            cout << "\n========================================" << endl;
            cout << "           INVENTORY MENU" << endl;
            cout << "========================================" << endl;
            
            // 1. Ekipman ve Cantayi Goster
            hero.printEquipment();
            hero.printInventory();

            cout << "----------------------------------------" << endl;
            cout << "Enter Item ID to Use/Equip (or -1 to Exit): ";
            
            int inputIndex;
            cin >> inputIndex;

            // Cikis
            if (inputIndex == -1) {
                inMenu = false;
                break;
            }

            const auto& inv = hero.getInventory();

            // Gecerlilik Kontrolu
            if (inputIndex < 0 || inputIndex >= inv.size()) {
                cout << "Invalid selection!" << endl;
                continue;
            }

            Item* selected = inv[inputIndex];
            
            // --- TUR KONTROLU VE EYLEM ---
            if (Weapon* w = dynamic_cast<Weapon*>(selected)) {
                hero.equipWeapon(w);
            }
            else if (Armor* a = dynamic_cast<Armor*>(selected)) {
                hero.equipArmor(a);
            }
            else if (Consumable* c = dynamic_cast<Consumable*>(selected)) {
                hero.useItem(inputIndex);
            }
            else {
                cout << "You cannot use/equip this item directly." << endl;
            }
        }
    }

public:
    Game() {
        cout << "Initializing Game Engine..." << endl;
        
        // Haritayi yukle (Yeni formatli txt dosyasini okuyacak)
        mapMgr.loadMap("Rooms.txt");
        
        // Baslangic Odasi: 1
        currentRoom = mapMgr.getRoom(1);
        
        if (currentRoom == nullptr) {
            cout << "CRITICAL ERROR: Room 1 not found!" << endl;
        }
    }

    // --- TEST ICIN HILE MODU ---
    void setupCheats() {
        cout << "\n>>> CHEAT MODE ON <<<" << endl;
        
        // Master Sword (ID: 199)
        Item* godSword = itemMgr.getItem(199);
        if (godSword) hero.addItem(godSword);
        
        // 3 Tane Iksir (ID: 1)
        for(int i=0; i<3; i++) {
            Item* potion = itemMgr.getItem(1);
            if(potion) hero.addItem(potion);
        }
        cout << ">>> Items added. Press 'i' to equip them!\n" << endl;
    }

    // --- OYUN DONGUSU ---
    void run() {
        if (!currentRoom) return;

        cout << "--- GAME START ---\n" << endl;
        bool isRunning = true;

        while (isRunning) {
            
            // --- 1. OTOMATIK SAVAS KONTROLU (COKLU DUSMAN) ---
            // 'monsterID' artik bir vektor oldugu icin '.empty()' ile kontrol ediyoruz.
            
            if (!currentRoom->monsterID.empty()) {
                
                vector<Monster*> battleGroup;

                // Vektordeki butun ID'leri donup canavarlari olusturuyoruz
                for (int mID : currentRoom->monsterID) {
                    Monster* enemy = mobMgr.getEnemy(mID);
                    if (enemy) {
                        battleGroup.push_back(enemy);
                    }
                }

                // Eger canavarlar basariyla olusturulduysa savasi baslat
                if (!battleGroup.empty()) {
                    cout << "\n>>> DANGER! You encounter " << battleGroup.size() << " enemies! <<<" << endl;

                    // CombatManager'a grubu gonderiyoruz
                    bool win = combatMgr.startBattle(&hero, battleGroup, itemMgr);

                    if (win) {
                        cout << "\n*** VICTORY ***" << endl;
                        // Odadaki canavar listesini tamamen temizle
                        currentRoom->monsterID.clear(); 
                    } else {
                        cout << "\n*** GAME OVER ***" << endl;
                        isRunning = false;
                        continue;
                    }
                }
            }
            // ------------------------------------------------

            // 2. ODA BILGISI
            cout << "\n--------------------------------" << endl;
            cout << "LOCATION: " << currentRoom->info << endl;
            
            // Yerdeki Esya Kontrolu
            if (currentRoom->itemID != -1) {
                Item* item = itemMgr.getItem(currentRoom->itemID);
                if (item) {
                    cout << "GROUND: You see a [" << item->getName() << "] here." << endl;
                    delete item; // Sadece ismini gostermek icin aldik, siliyoruz
                }
            }
            cout << "--------------------------------" << endl;

            // 3. MENU VE KOMUTLAR
            hero.printStats(); // HP ve Durumu goster
            cout << "Action (w,a,s,d: Move | i: Inventory | e: Interact | q: Quit): ";
            
            char input;
            cin >> input;

            switch (input) {
                case 'w': move(currentRoom->n); break;
                case 's': move(currentRoom->s); break;
                case 'd': move(currentRoom->e); break;
                case 'a': move(currentRoom->w); break;
                
                case 'i': manageInventory(); break; // Envanter Yonetimi
                case 'e': interactWithRoom(); break; // Esya Toplama
                case 'q': isRunning = false; break;

                default: cout << "Invalid command." << endl;
            }
        }
    }

    // --- YARDIMCI METOTLAR ---
    void move(int id) {
        if (id == -1) cout << "Blocked." << endl;
        else {
            Room* next = mapMgr.getRoom(id);
            if (next) {
                currentRoom = next;
                cout << "Moving..." << endl;
            } else {
                cout << "Error: Room not found." << endl;
            }
        }
    }

    void interactWithRoom() {
        // Savas artik otomatik. Burasi sadece item toplar.
        if (currentRoom->itemID != -1) {
            Item* item = itemMgr.getItem(currentRoom->itemID);
            if (item) {
                if (hero.addItem(item)) {
                    cout << "Picked up " << item->getName() << endl;
                    currentRoom->itemID = -1; // Yerdeki esyayi sil
                } else {
                    cout << "Inventory full!" << endl;
                    delete item;
                }
            }
        } else {
            cout << "Nothing here." << endl;
        }
    }
};