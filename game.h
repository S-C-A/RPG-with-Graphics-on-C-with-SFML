#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <algorithm> 

// BAGIMLILIKLAR
#include "Player.h"
#include "ItemManager.h"
#include "enemyManager.h"
#include "room.h"
#include "combat.h" 
#include "NPCManager.h" 

using namespace std;

class Game {
private:
    Player hero;
    ItemManager itemMgr;
    EnemyManager mobMgr;
    MapManager mapMgr;
    CombatManager combatMgr;
    NPCManager npcMgr;

    Room* currentRoom;

    string formatText(string text) {
        std::replace(text.begin(), text.end(), '_', ' ');
        return text;
    }

public:
    Game() {
        mapMgr.loadMap("Rooms.txt");
        currentRoom = mapMgr.getRoom(1); 
        if (!currentRoom) std::cerr << "CRITICAL ERROR: Room 1 not found!" << std::endl;

        // BASLANGIC ESYALARI
        Item* sword = itemMgr.getItem(100);
        if (sword) hero.addItem(sword);

        Item* armor = itemMgr.getItem(200);
        if (armor) hero.addItem(armor);

        Item* potion = itemMgr.getItem(1);
        if (potion) hero.addItem(potion);

        Item* key = itemMgr.getItem(300);
        if (key) hero.addItem(key);
    }

    string getItemDesc(int index) {
        if (index < 0 || index >= hero.getInventory().size()) return "";
        return hero.getInventory()[index]->getInfo();
    }

    string playerUseItem(int index) {
        if (index < 0 || index >= hero.getInventory().size()) return ""; 
        return hero.useItem(index);
    }

    Player& getPlayer() { return hero; }
    Room* getCurrentRoom() { return currentRoom; }

    // --- ACTIONS ---

    // 1. Hareket Mantigi (DUZELTILDI)
    string attemptMove(int nextRoomID) {
        if (nextRoomID == -1) {
            return "The path is blocked.";
        }
        
        Room* next = mapMgr.getRoom(nextRoomID);
        if (next) {
            currentRoom = next;
            // DUZELTME BURADA:
            // Eskiden sadece 'currentRoom->info' donduruyorduk.
            // Simdi 'lookAtRoom()' cagiriyoruz ki yerdeki esya bilgisini de eklesin.
            return lookAtRoom(); 
        }
        return "Unknown path.";
    }

    // 2. Etkilesim
    string tryPickupItem() {
        if (currentRoom->itemID != -1) {
            Item* item = itemMgr.getItem(currentRoom->itemID);
            if (item) {
                if (hero.addItem(item)) {
                    currentRoom->itemID = -1; 
                    // Esyayi aldiktan sonra odaya tekrar bakalim (yazi guncellensin)
                    // "Picked up X" yazisi ustune odanin temiz hali gelir.
                    // Ama burada sadece bilgilendirme donelim, typer halleder.
                    return "Picked up [" + item->getName() + "]";
                } else {
                    delete item; 
                    return "Inventory is full.";
                }
            }
        }
        return "Nothing here.";
    }

    // 3. Odaya Bakis (Akilli Versiyon)
    string lookAtRoom() {
        if(!currentRoom) return "Void.";

        // Odanin kendi aciklamasi
        string desc = formatText(currentRoom->info);

        // Yerde esya var mi?
        if (currentRoom->itemID != -1) {
            Item* tempItem = itemMgr.getItem(currentRoom->itemID);
            if (tempItem) {
                // Alt satira esya bilgisini ekle
                desc += "\nYou see a [" + tempItem->getName() + "] here.";
                delete tempItem; 
            }
        }
        return desc;
    }
    
    // 4. Cheat
    void addGoldCheat(int amount) {
        hero.goldChange(amount);
    }
};