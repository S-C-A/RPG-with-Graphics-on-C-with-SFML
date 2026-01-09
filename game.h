#pragma once
#include <iostream>
#include <vector>
#include <ctime>

// Tum bagimliliklar
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

    // --- ENVANTER MENUSU (YENI) ---
    void manageInventory() {
        bool inMenu = true;
        
        while (inMenu) {
            cout << "\n========================================" << endl;
            cout << "           INVENTORY MENU" << endl;
            cout << "========================================" << endl;
            
            // 1. Mevcut Ekipmani Goster
            hero.printEquipment();

            // 2. Cantayi Listele
            // Player.h'daki fonksiyonu kullaniyoruz, listeyi yazdiriyor.
            hero.printInventory();

            cout << "----------------------------------------" << endl;
            cout << "Enter Item ID to Use/Equip (or -1 to Exit): ";
            
            int inputIndex;
            cin >> inputIndex;

            // Cikis Kontrolu
            if (inputIndex == -1) {
                inMenu = false;
                break;
            }

            // Player.h'da printInventory 0'dan baslayarak (0,1,2...) yazdiriyor.
            // Kullanici da buna gore girecek.
            const auto& inv = hero.getInventory();

            if (inputIndex < 0 || inputIndex >= inv.size()) {
                cout << "Invalid selection!" << endl;
                continue;
            }

            // Secilen esyayi alalim
            Item* selected = inv[inputIndex];
            
            // --- TÜR KONTROLÜ VE EYLEM ---

            // A. SILAH MI?
            if (Weapon* w = dynamic_cast<Weapon*>(selected)) {
                // Player.h'daki fonksiyonu cagiriyoruz.
                // O fonksiyon esyayi cantadan silip equippedWeapon yapiyor.
                hero.equipWeapon(w);
            }
            // B. ZIRH MI?
            else if (Armor* a = dynamic_cast<Armor*>(selected)) {
                hero.equipArmor(a);
            }
            // C. CONSUMABLE (IKSIR) MI?
            else if (Consumable* c = dynamic_cast<Consumable*>(selected)) {
                // useItem fonksiyonu iceride 'use' cagirip siliyor.
                hero.useItem(inputIndex);
            }
            // D. DIGER
            else {
                cout << "You cannot use/equip this item directly." << endl;
            }
            
            // Dongu basa doner, boylece baska esyalar da secebilirsin.
        }
    }

public:
    Game() {
        cout << "Initializing Game Engine..." << endl;
        
        mapMgr.loadMap("Rooms.txt");
        currentRoom = mapMgr.getRoom(1);
        
        if (currentRoom == nullptr) {
            cout << "CRITICAL ERROR: Room 1 not found!" << endl;
        }
    }

    // --- TEST ICIN HILE ---
    void setupCheats() {
        cout << "\n>>> CHEAT MODE ON: Adding Master Sword & Potions... <<<" << endl;

        // 1. Master Sword (ID: 199)
        Item* godSword = itemMgr.getItem(199);
        if (godSword) hero.addItem(godSword);

        // 2. Demir Kilic (ID: 101 - Karsilastirma icin)
        Item* ironSword = itemMgr.getItem(101);
        if (ironSword) hero.addItem(ironSword);

        // 3. Iksirler (ID: 1)
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
            
            // 1. OTOMATIK SAVAS (Combat.h ile baglanti)
            if (currentRoom->monsterID != -1) {
                Monster* enemy = mobMgr.getEnemy(currentRoom->monsterID);
                
                if (enemy) {
                    vector<Monster*> group;
                    group.push_back(enemy);

                    // CombatManager cagiriliyor (ItemManager referansi ile)
                    bool win = combatMgr.startBattle(&hero, group, itemMgr);

                    if (win) {
                        cout << "\n*** VICTORY ***" << endl;
                        currentRoom->monsterID = -1; // Oda temizlendi
                    } else {
                        cout << "\n*** GAME OVER ***" << endl;
                        isRunning = false;
                        continue;
                    }
                }
            }

            // 2. ODA BILGISI
            cout << "\n--------------------------------" << endl;
            cout << "LOCATION: " << currentRoom->info << endl;
            
            if (currentRoom->itemID != -1) {
                Item* item = itemMgr.getItem(currentRoom->itemID);
                if (item) {
                    cout << "GROUND: You see a [" << item->getName() << "] here." << endl;
                    delete item;
                }
            }
            cout << "--------------------------------" << endl;

            // 3. MENU
            hero.printStats(); // HP ve Statlari gorelim
            cout << "Action (w,a,s,d: Move | i: Inventory | e: Interact | q: Quit): ";
            
            char input;
            cin >> input;

            switch (input) {
                case 'w': move(currentRoom->n); break;
                case 's': move(currentRoom->s); break;
                case 'd': move(currentRoom->e); break;
                case 'a': move(currentRoom->w); break;
                
                case 'i': 
                    manageInventory(); // YENI FONKSİYON
                    break;
                
                case 'e':
                    interactWithRoom();
                    break;

                case 'q': 
                    isRunning = false; 
                    break;

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
            }
        }
    }

    void interactWithRoom() {
        if (currentRoom->itemID != -1) {
            Item* item = itemMgr.getItem(currentRoom->itemID);
            if (item) {
                if (hero.addItem(item)) {
                    cout << "Picked up " << item->getName() << endl;
                    currentRoom->itemID = -1;
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