#pragma once
#include <iostream>
#include <vector>
#include <ctime>

#include "player.h"
#include "ItemManager.h"
#include "enemyManager.h"
#include "room.h"
#include "combat.h" // Combat sistemini ekledik

using namespace std;

class Game {
private:
    Player hero;
    ItemManager itemMgr;
    EnemyManager mobMgr;
    MapManager mapMgr;
    CombatManager combatMgr; // Savas yoneticisi

    Room* currentRoom;

public:
    Game() {
        cout << "Game Engine Initializing..." << endl;
        
        // Haritayi yukle
        mapMgr.loadMap("Rooms.txt");

        // Baslangic odasi ID: 1
        currentRoom = mapMgr.getRoom(1);
        
        if (currentRoom == nullptr) {
            cout << "ERROR: Starting room not found! (ID: 1)" << endl;
        }
    }

    // --- TEST VE HILE FONKSIYONU ---
    // Bunu main.cpp icinden cagiracagiz
    void setupCheats() {
        cout << "\n>>> CHEAT MODE ACTIVATED <<<" << endl;
        
        // 1. Master Sword Ekle (ID: 199)
        Item* godWeapon = itemMgr.getItem(199);
        if (godWeapon) {
            hero.addItem(godWeapon);
            cout << ">>> ADDED: " << godWeapon->getName() << endl;
        }

        // 2. Cantayi Iksirle Doldur (ID: 1)
        // Ornegin 5 tane iksir verelim
        for (int i = 0; i < 5; i++) {
            Item* potion = itemMgr.getItem(1);
            if (potion) {
                hero.addItem(potion);
            }
        }
        cout << ">>> ADDED: 5x Health Potions" << endl;
        cout << ">>> Inventory Ready for Testing.\n" << endl;
    }

    // --- MAIN GAME LOOP ---
    void run() {
        if (currentRoom == nullptr) return;

        cout << "--- GAME STARTED ---\n" << endl;
        bool isRunning = true;

        while (isRunning) {
            
            // --- OTOMATIK SAVAS KONTROLU ---
            if (currentRoom->monsterID != -1) {
                Monster* enemy = mobMgr.getEnemy(currentRoom->monsterID);
                
                if (enemy != nullptr) {
                    // Dusmani savas grubuna ekle
                    vector<Monster*> battleGroup;
                    battleGroup.push_back(enemy);

                    // Savasi Baslat
                    // ONEMLI: itemMgr parametresini de gonderiyoruz!
                    bool win = combatMgr.startBattle(&hero, battleGroup, itemMgr);

                    if (win) {
                        cout << "\n*** VICTORY ***" << endl;
                        
                        // Odadan canavari sil (Temizle)
                        currentRoom->monsterID = -1;
                        
                        // Not: Ganimet (Loot) islemini CombatManager halletti.
                    } 
                    else {
                        cout << "\n*** GAME OVER ***" << endl;
                        isRunning = false; 
                        continue; 
                    }
                }
            }

            // 1. ODA BILGISI
            cout << "\n--------------------------------" << endl;
            cout << "LOCATION: " << currentRoom->info << endl;
            
            // Yerde esya var mi?
            if (currentRoom->itemID != -1) {
                Item* tempItem = itemMgr.getItem(currentRoom->itemID);
                if (tempItem) {
                    cout << "GROUND: There is a [" << tempItem->getName() << "] here." << endl;
                    delete tempItem; 
                }
            }
            cout << "--------------------------------" << endl;

            // 2. KOMUT AL
            cout << "Action (w,a,s,d: Move | i: Inventory | e: Interact | q: Quit): ";
            char input;
            cin >> input;

            // 3. KOMUTU ISLE
            switch (input) {
                case 'w': move(currentRoom->n); break;
                case 's': move(currentRoom->s); break;
                case 'd': move(currentRoom->e); break;
                case 'a': move(currentRoom->w); break;
                
                case 'i': 
                    hero.printInventory(); 
                    break;
                
                case 'e':
                    interactWithRoom();
                    break;

                case 'q': 
                    cout << "Exiting game..." << endl;
                    isRunning = false;
                    break;

                default: cout << "Invalid command." << endl;
            }
        }
    }

    // --- YARDIMCI FONKSIYONLAR ---

    void move(int roomID) {
        if (roomID == -1) {
            cout << "You can't go that way (Wall)." << endl;
        } else {
            Room* nextRoom = mapMgr.getRoom(roomID);
            if (nextRoom != nullptr) {
                currentRoom = nextRoom;
                cout << "Walking..." << endl;
            } else {
                cout << "ERROR: Room not found!" << endl;
            }
        }
    }

    // Sadece yerdeki esyalari almak icin kullaniliyor artik
    void interactWithRoom() {
        if (currentRoom->itemID != -1) {
            Item* item = itemMgr.getItem(currentRoom->itemID);
            if (item) {
                cout << ">> Picking up " << item->getName() << "..." << endl;
                
                if (hero.addItem(item)) {
                    currentRoom->itemID = -1; 
                    cout << "You picked up the " << item->getName() << "." << endl;
                } else {
                    cout << "Inventory full! Could not pick up item." << endl;
                    delete item; 
                }
            }
        } else {
            cout << "Nothing to pick up here." << endl;
        }
    }
};