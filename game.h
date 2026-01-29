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
// Dont forget to add new inventory full menu when managing enemy loot.
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

    NPC* activeNPC = nullptr;           // Su an kiminle konusuyoruz?
    DialogueNode* activeNode = nullptr; // Hangi cumledeyiz?

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

    string playerDropItem(int index) {
        // Player.h icindeki guncelledigimiz fonksiyonu cagirir ve mesaji doner
        return hero.removeItem(index);
    }

    Player& getPlayer() { return hero; }
    Room* getCurrentRoom() { return currentRoom; }

    NPC* getRoomNPC() {
        if (currentRoom && currentRoom->npcID != -1) {
            return npcMgr.getNPC(currentRoom->npcID);
        }
        return nullptr;
    }

    // --- DIYALOG SISTEMI ---

    //Ilk defa konusulmus mu kontrol
    NPC* checkForAutoDialogue() {
        if (currentRoom && currentRoom->npcID != -1) {
            NPC* npc = npcMgr.getNPC(currentRoom->npcID);
            
            // Eger NPC varsa VE daha once tanismadiysak (hasMet == false)
            if (npc && !npc->hasMet()) {
                // Tanisma durumunu guncelle (Artik tanistik)
                npc->setMet(true);
                return npc; // Konusulacak NPC'yi dondur
            }
        }
        return nullptr; // Otomatik konusma yok
    }

    // A) Konusmayi Baslat
    // Geriye NPC'nin ilk cumlesini doner.
    string startDialogue(NPC* npc) {
        if (!npc) return "";
        
        activeNPC = npc;
        // NPC'nin hafizasindaki kaldigi yerden (Root) basla
        activeNode = activeNPC->getDialogue(activeNPC->getRootNode());
        
        if (activeNode) return activeNode->npcText;
        return "...";
    }

    // B) Secenek Metinlerini Al (Main.cpp ekrana yazdirmak icin kullanacak)
    vector<string> getDialogueOptions() {
        vector<string> labels;
        if (!activeNode) return labels;

        for (const auto& opt : activeNode->options) {
            labels.push_back(opt.text); // Sadece metinleri gonderiyoruz
        }
        
        // Eger hic secenek yoksa ama konusma bitmediyse "Devam" secenegi koy
        if (labels.empty()) {
            labels.push_back("Continue...");
        }
        return labels;
    }

    // C) Oyuncu bir secenegi sectiginde ne olacak?
    // index: Oyuncunun tikladigi siradaki secenek (0, 1, 2...)
    // Donus: Yeni NPC metni (veya diyalog bittiyse BOS string)
    string selectDialogueOption(int index) {
        if (!activeNPC || !activeNode) return "";

        int nextID = -1;

        // Secenek varsa, secilenin nereye gittigine bak (NextNodeID)
        if (!activeNode->options.empty()) {
            if (index >= 0 && index < activeNode->options.size()) {
                nextID = activeNode->options[index].nextNodeID;
            } else {
                return ""; // Hatali tiklama
            }
        } 
        // Secenek yoksa (Continue durumu), genelde -1 ile biter veya ozel mantik
        else {
             nextID = -1; 
        }

        // --- DIYALOG BITISI ---
        if (nextID == -1) {
            activeNPC = nullptr;
            activeNode = nullptr;
            return ""; // Bos string donmesi "Konusma Bitti" demektir.
        }

        // --- YENI NODE GECIS ---
        activeNode = activeNPC->getDialogue(nextID);
        
        // --- OLAYLARI ISLE (Item verme, Heal vb.) ---
        return processNodeEvents(activeNode);
    }

    // D) Olay Isleyici (Internal - Backend)
    string processNodeEvents(DialogueNode* node) {
        if (!node) return "";

        // 1. Root Degistirme (Hikaye ilerlemesi - Orn: Artik beni tanidin)
        if (node->changeRootTo != -1) {
            activeNPC->setRootNode(node->changeRootTo);
        }

        // 2. Aksiyonlar
        if (node->actionType == EVENT_GIVE_ITEM) {
            Item* gift = itemMgr.getItem(node->actionValue);
            if (gift) {
                if (hero.addItem(gift)) {
                    // Basarili
                } else {
                    currentRoom->itemID = node->actionValue; // Yer yoksa yere at
                    delete gift;
                }
            }
        }
        else if (node->actionType == EVENT_HEAL) {
            hero.heal(node->actionValue);
        }
        else if (node->actionType == EVENT_START_COMBAT) {
            return "COMBAT_START"; // Main.cpp bunu yakalayip savas moduna gececek
        }

        return node->npcText;
    }

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
                    return "Inventory is full. Right click an item to discard it.";
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

        // 2. (YENI) Odada NPC var mi?
        NPC* npc = getRoomNPC();
        if (npc) {
            desc += "\n\n[" + npc->getName() + "] is standing here.";
        }

        return desc;
    }
    
    // 4. Cheat
    void addGoldCheat(int amount) {
        hero.goldChange(amount);
    }
};