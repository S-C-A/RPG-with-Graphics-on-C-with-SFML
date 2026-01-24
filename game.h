#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <algorithm> // replace fonksiyonu icin gerekli

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

    // --- METIN TEMIZLEYICI (YENI) ---
    // "Dark_Corridor" gibi metinleri "Dark Corridor" haline cevirir.
    string formatText(string text) {
        std::replace(text.begin(), text.end(), '_', ' ');
        return text;
    }

public:
    Game() {
        mapMgr.loadMap("Rooms.txt");
        currentRoom = mapMgr.getRoom(1); 
        if (!currentRoom) std::cerr << "CRITICAL ERROR: Room 1 not found!" << std::endl;

        Item* sword = itemMgr.getItem(100);
        if (sword) hero.addItem(sword);

        // 2. Deri Zirh (ID: 200)
        Item* armor = itemMgr.getItem(200);
        if (armor) hero.addItem(armor);

        // 3. Kucuk Iksir (ID: 1)
        Item* potion = itemMgr.getItem(1);
        if (potion) hero.addItem(potion);
    }

    string playerUseItem(int index) {
        // Hata Kontrolu: Index gecerli mi?
        if (index < 0 || index >= hero.getInventory().size()) {
            return ""; // Gecersiz tiklama
        }

        Item* item = hero.getInventory()[index];
        string itemName = item->getName();
        string resultMsg = "";

        // Ne tur bir esya olduguna bakalim (Mesaj icin)
        // NOT: Dynamic cast, esyanin turunu kontrol eder.
        if (dynamic_cast<Weapon*>(item) || dynamic_cast<Armor*>(item)) {
            resultMsg = "Equipped [" + itemName + "]";
        } else {
            resultMsg = "Used [" + itemName + "]";
        }

        // Islemi Yap (Player.h icindeki useItem her seyi hallediyor: silme, kusanma vs.)
        hero.useItem(index);

        return resultMsg;
    }

    // --- GETTERS ---
    Player& getPlayer() { return hero; }
    Room* getCurrentRoom() { return currentRoom; }

    // --- ACTIONS ---

    // 1. Hareket Mantigi (Guncellendi)
    string attemptMove(int nextRoomID) {
        if (nextRoomID == -1) {
            return "The path is blocked.";
        }
        
        Room* next = mapMgr.getRoom(nextRoomID);
        if (next) {
            currentRoom = next;
            // Artik "Moved to:" demiyoruz, direkt odanin ismini/aciklamasini veriyoruz.
            return formatText(currentRoom->info);
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
                    return "Picked up [" + item->getName() + "]";
                } else {
                    delete item; 
                    return "Inventory is full.";
                }
            }
        }
        return "Nothing here.";
    }

    // 3. Baslangic Bakisi (Guncellendi)
    string lookAtRoom() {
        if(currentRoom) return formatText(currentRoom->info);
        return "Void.";
    }
    
    // 4. Cheat
    void addGoldCheat(int amount) {
        hero.goldChange(amount);
    }
};