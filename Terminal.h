#pragma once
#include <iostream>
#include <vector>
#include <ctime>

// TUM BAGIMLILIKLAR
#include "player.h"
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

    // --- DIYALOG SISTEMI ---
    void startDialogue(NPC* npc) {
        cout << "\n========================================" << endl;
        cout << ">>> Speaking with: " << npc->getName() << endl;
        cout << "========================================" << endl;

        int currentNodeID = npc->getRootNode(); 
        bool talking = true;

        while (talking) {
            DialogueNode* node = npc->getDialogue(currentNodeID);
            if (node == nullptr) break;

            // 1. ROOT DEGISTIRME
            if (node->changeRootTo != -1) {
                npc->setRootNode(node->changeRootTo);
            }

            // 2. EKRANA YAZ
            cout << "\n" << npc->getName() << ": \"" << node->npcText << "\"\n" << endl;

            // 3. OLAYLARI UYGULA (EVENTS)
            if (node->actionType != EVENT_NONE) {
                
                // A) ESYA VERME
                if (node->actionType == EVENT_GIVE_ITEM) {
                    Item* gift = itemMgr.getItem(node->actionValue);
                    if (gift) {
                        if (hero.addItem(gift)) {
                            cout << ">>> RECEIVED ITEM: " << gift->getName() << " <<<" << endl;
                        } else {
                            cout << ">>> Inventory Full! Dropped " << gift->getName() << " on ground. <<<" << endl;
                            currentRoom->itemID = node->actionValue; 
                            delete gift; 
                        }
                    }
                }
                // B) IYILESTIRME
                else if (node->actionType == EVENT_HEAL) {
                    hero.heal(node->actionValue);
                    cout << ">>> You feel refreshed! (+" << node->actionValue << " HP) <<<" << endl;
                }
                // C) SAVAS BASLATMA
                else if (node->actionType == EVENT_START_COMBAT) {
                    cout << ">>> SUDDEN ATTACK! The conversation turns violent! <<<" << endl;
                    
                    talking = false; 
                    
                    Monster* enemy = mobMgr.getEnemy(node->actionValue);
                    if (enemy) {
                        vector<Monster*> group;
                        group.push_back(enemy);
                        combatMgr.startBattle(&hero, group, itemMgr);
                    }
                    break;
                }
                // D) DUKKAN ACMA
                else if (node->actionType == EVENT_OPEN_SHOP) {
                    openShopMenu(npc);
                }
            }

            // Konusma bitti mi?
            if (node->options.empty()) {
                cout << "(Conversation ends.)" << endl;
                cin.ignore(); cin.get(); 
                talking = false;
                break;
            }

            // Secenekler
             for (size_t i = 0; i < node->options.size(); i++) {
                cout << i + 1 << ": " << node->options[i].text << endl;
            }

            int choice;
            cout << "> ";
            cin >> choice;
            
            if (choice < 1 || choice > node->options.size()) { cout << "Invalid choice." << endl; continue; }
            int nextID = node->options[choice - 1].nextNodeID;
            if (nextID == -1) { talking = false; } 
            else { currentNodeID = nextID; }
        }
        
        if(!npc->hasMet()) npc->setMet(true);
        cout << "----------------------------------------\n" << endl;
    }

    // --- DUKKAN SISTEMI ---
    void openShopMenu(NPC* merchant) {
        const vector<ShopItem>& items = merchant->getShopInventory();
        
        if (items.empty()) {
            cout << "Merchant: I have nothing to sell right now." << endl;
            return;
        }

        bool shopping = true;
        while (shopping) {
            cout << "\n=== " << merchant->getName() << "'s SHOP ===" << endl;
            cout << "Your Gold: " << hero.getGold() << endl;
            cout << "---------------------------" << endl;

            for (size_t i = 0; i < items.size(); i++) {
                Item* temp = itemMgr.getItem(items[i].itemID);
                if (temp) {
                    cout << i + 1 << ". " << temp->getName() 
                         << " (" << items[i].price << " G)" << endl;
                    delete temp; 
                }
            }
            cout << "0. Exit Shop" << endl;
            cout << "---------------------------" << endl;
            cout << "Buy Item #: ";

            int choice;
            cin >> choice;

            if (choice == 0) {
                shopping = false;
                cout << "Merchant: Come back soon!" << endl;
            }
            else if (choice > 0 && choice <= items.size()) {
                ShopItem selected = items[choice - 1];
                
                if (hero.getGold() >= selected.price) {
                    Item* newItem = itemMgr.getItem(selected.itemID);
                    if (hero.addItem(newItem)) {
                        hero.goldChange(-selected.price); // Parayi dus
                        cout << ">>> PURCHASE SUCCESSFUL! <<<" << endl;
                    } else {
                        cout << ">>> Inventory Full! Money refunded. <<<" << endl;
                        // Para dusmedik o yuzden iadeye gerek yok, itemi sil yeter.
                        delete newItem;
                    }
                } else {
                    cout << ">>> Not enough gold! <<<" << endl;
                }
            } else {
                cout << "Invalid selection." << endl;
            }
        }
    }

// --- ENVANTER MENUSU (GUNCEL) ---
    void manageInventory() {
        bool inMenu = true;
        while (inMenu) {
            cout << "\n--- INVENTORY ---" << endl;
            hero.printEquipment();
            hero.printInventory();
            
            // DEGISTI: Soru metni
            cout << "Enter Item # to Use/Equip (0 to Exit): ";
            int input; 
            cin >> input;

            // DEGISTI: Cikis kosulu artik 0
            if (input == 0) break;

            // DEGISTI: Kullanici 1 dedi, ama bilgisayar icin o 0. eleman.
            // Bu yuzden 1 cikariyoruz.
            int realIndex = input - 1;

            const auto& inv = hero.getInventory();
            
            // DEGISTI: Artik "realIndex" kontrol ediliyor
            if (realIndex >= 0 && realIndex < inv.size()) {
                Item* selected = inv[realIndex];
                
                if (Weapon* w = dynamic_cast<Weapon*>(selected)) hero.equipWeapon(w);
                else if (Armor* a = dynamic_cast<Armor*>(selected)) hero.equipArmor(a);
                else if (Consumable* c = dynamic_cast<Consumable*>(selected)) hero.useItem(realIndex);
            } else {
                cout << "Invalid selection." << endl;
            }
        }
    }

public:
    Game() {
        cout << "Initializing Game Engine..." << endl;
        
        // 1. Haritayi Yukle (NPC'ler artik Rooms.txt icinden okunuyor)
        mapMgr.loadMap("Rooms.txt");
        
        // 2. Baslangic Odasini Al
        currentRoom = mapMgr.getRoom(1); 
        
        if (!currentRoom) cout << "CRITICAL ERROR: Room 1 not found!" << endl;

        // NOT: Artik burada manuel "npcID.push_back" yapmiyoruz.
        // Cunku MapManager dosya okurken npcID degiskenini set etti.
    }

    void setupCheats() {
        Item* godSword = itemMgr.getItem(199);
        if (godSword) hero.addItem(godSword);
        hero.goldChange(1000); // Test icin para
        cout << ">>> CHEAT MODE ON: God Sword + 1000 Gold added.\n" << endl;
    }

    void run() {
        if (!currentRoom) return;
        
        setupCheats();

        cout << "--- GAME START ---\n" << endl;
        bool isRunning = true;

        while (isRunning) {
            
            // 1. SAVAS KONTROLU (Oncelik Dusmanda)
            if (!currentRoom->monsterID.empty()) {
                vector<Monster*> battleGroup;
                for (int mID : currentRoom->monsterID) {
                    Monster* enemy = mobMgr.getEnemy(mID);
                    if (enemy) battleGroup.push_back(enemy);
                }

                if (!battleGroup.empty()) {
                    cout << "\n>>> ENEMIES SPOTTED! <<<" << endl;
                    bool win = combatMgr.startBattle(&hero, battleGroup, itemMgr);
                    if (win) currentRoom->monsterID.clear();
                    else { isRunning = false; continue; }
                }
            }

            // 2. NPC KONTROLU (Savas yoksa calisir)
            // DEGISTI: Vektor kontrolu (.empty()) yerine int kontrolu (!= -1)
            if (currentRoom->npcID != -1) {
                
                int npcID = currentRoom->npcID; // [0] kaldirildi
                NPC* npc = npcMgr.getNPC(npcID);

                if (npc) {
                    // Eger ilk kez karsilasiyorsak OTOMATIK konus
                    if (!npc->hasMet()) {
                        cout << "\n>>> Someone approaches you..." << endl;
                        startDialogue(npc);
                    } else {
                        cout << "INFO: " << npc->getName() << " is standing here." << endl;
                    }
                }
            }

            // 3. ODA BILGISI
            cout << "\n--------------------------------" << endl;
            cout << "LOCATION: " << currentRoom->info << endl;
            if (currentRoom->itemID != -1) {
                Item* item = itemMgr.getItem(currentRoom->itemID);
                if (item) {
                    cout << "GROUND: You see [" << item->getName() << "]" << endl;
                    delete item;
                }
            }
            cout << "--------------------------------" << endl;

            // 4. MENU
            hero.printStats();
            cout << "Action (w,a,s,d: Move | i: Inv | t: Talk | e: Interact | q: Quit): ";
            
            char input; cin >> input;

            switch (input) {
                case 'w': move(currentRoom->n); break;
                case 's': move(currentRoom->s); break;
                case 'd': move(currentRoom->e); break;
                case 'a': move(currentRoom->w); break;
                
                case 'i': manageInventory(); break;
                
                case 't': // TALK (KONUSMA TUSU)
                    // DEGISTI: Vektor kontrolu yerine int kontrolu
                    if (currentRoom->npcID != -1) {
                        NPC* npc = npcMgr.getNPC(currentRoom->npcID);
                        if(npc) startDialogue(npc);
                    } else {
                        cout << "There is no one to talk to." << endl;
                    }
                    break;

                case 'e': interactWithRoom(); break;
                case 'q': isRunning = false; break;
                default: cout << "Invalid command." << endl;
            }
        }
    }

    void move(int id) {
        if (id == -1) cout << "Blocked." << endl;
        else {
            Room* next = mapMgr.getRoom(id);
            if (next) currentRoom = next;
        }
    }

    void interactWithRoom() {
        if (currentRoom->itemID != -1) {
            Item* item = itemMgr.getItem(currentRoom->itemID);
            if (item && hero.addItem(item)) {
                cout << "Picked up " << item->getName() << endl;
                currentRoom->itemID = -1;
            } else delete item;
        } else cout << "Nothing here." << endl;
    }
};