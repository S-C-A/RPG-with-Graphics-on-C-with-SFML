#pragma once
#include <iostream>
#include <vector>
#include <string>

// BAGIMLILIKLAR
// Bu dosyalarin proje klasorunde oldugundan emin ol
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

public:
    Game() {
        // Hata ciktilari icin cerr kullaniyoruz
        mapMgr.loadMap("Rooms.txt");
        
        // Baslangic Odasi (ID: 1)
        currentRoom = mapMgr.getRoom(1); 
        
        if (!currentRoom) {
            std::cerr << "CRITICAL ERROR: Room 1 not found!" << std::endl;
        }
    }

    // --- GETTERS ---
    // Main dosyasinin oyuncu verilerine (HP, Gold vb.) ulasmasi icin
    Player& getPlayer() { return hero; }
    
    // Main dosyasinin odaya (canavar var mi vb.) ulasmasi icin
    Room* getCurrentRoom() { return currentRoom; }

    // --- ACTIONS (EMIRLER) ---

    // 1. Hareket Mantigi (Yon tusuna basinca bu cagrilacak)
    string attemptMove(int nextRoomID) {
        if (nextRoomID == -1) {
            return "The path is blocked.";
        }
        
        Room* next = mapMgr.getRoom(nextRoomID);
        if (next) {
            currentRoom = next;
            return "Moved to: " + currentRoom->info;
        }
        return "Unknown path.";
    }

    // 2. Etkilesim (Yerdeki esyayi alma)
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

    // 3. Baslangic Bakisi
    string lookAtRoom() {
        if(currentRoom) return currentRoom->info;
        return "Void.";
    }
    
    // 4. Cheat (Test icin)
    void addGoldCheat(int amount) {
        hero.goldChange(amount);
    }
};