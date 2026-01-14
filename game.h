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
#include "NPCManager.h" // YENI: NPC Yoneticisi eklendi

using namespace std;

class Game {
private:
    Player hero;
    ItemManager itemMgr;
    EnemyManager mobMgr;
    MapManager mapMgr;
    CombatManager combatMgr;
    NPCManager npcMgr; // YENI: NPC Yoneticisi

    Room* currentRoom;

    // --- DIYALOG SISTEMI (YENI) ---
    void startDialogue(NPC* npc) {
        cout << "\n========================================" << endl;
        cout << ">>> Speaking with: " << npc->getName() << endl;
        cout << "========================================" << endl;

        int currentNodeID = npc->getRootNode(); 
        bool talking = true;

        while (talking) {
            DialogueNode* node = npc->getDialogue(currentNodeID);
            if (node == nullptr) break;

            // 1. ROOT DEGISTIRME (Hafiza)
            if (node->changeRootTo != -1) {
                npc->setRootNode(node->changeRootTo);
            }

            // 2. EKRANA YAZ
            cout << "\n" << npc->getName() << ": \"" << node->npcText << "\"\n" << endl;

            // 3. OLAYLARI UYGULA (EVENTS) --- YENI KISIM ---
            if (node->actionType != EVENT_NONE) {
                
                // A) ESYA VERME
                if (node->actionType == EVENT_GIVE_ITEM) {
                    Item* gift = itemMgr.getItem(node->actionValue);
                    if (gift) {
                        if (hero.addItem(gift)) {
                            cout << ">>> RECEIVED ITEM: " << gift->getName() << " <<<" << endl;
                        } else {
                            cout << ">>> Inventory Full! Dropped " << gift->getName() << " on ground. <<<" << endl;
                            // Odaya birakma mantigi eklenebilir
                            currentRoom->itemID = node->actionValue; 
                            delete gift; // Pointer'i siliyoruz cunku envantere giremedi
                        }
                    }
                }
                // B) IYILESTIRME
                else if (node->actionType == EVENT_HEAL) {
                    hero.heal(node->actionValue); // Player'a heal metodu eklemelisin yoksa hata verir!
                    cout << ">>> You feel refreshed! (+" << node->actionValue << " HP) <<<" << endl;
                }
                // C) SAVAS BASLATMA
                else if (node->actionType == EVENT_START_COMBAT) {
                    cout << ">>> SUDDEN ATTACK! The conversation turns violent! <<<" << endl;
                    
                    // Konusmayi bitir
                    talking = false; 
                    
                    // Dusmani hazirla
                    Monster* enemy = mobMgr.getEnemy(node->actionValue);
                    if (enemy) {
                        vector<Monster*> group;
                        group.push_back(enemy);
                        combatMgr.startBattle(&hero, group, itemMgr);
                    }
                    break; // Donguden cik
                }
                else if (node->actionType == EVENT_OPEN_SHOP) {
                    openShopMenu(npc);
                }

                // Olay tek seferlik olsun diye actionType'i sifirlayabiliriz (Opsiyonel)
                // node->actionType = EVENT_NONE; 
            }
            // ------------------------------------------------

            if (node->options.empty()) {
                cout << "(Conversation ends.)" << endl;
                cin.ignore(); cin.get(); 
                talking = false;
                break;
            }

            // ... (Secenekleri gosterme ve girdi alma kismi ayni) ...
             for (size_t i = 0; i < node->options.size(); i++) {
                cout << i + 1 << ": " << node->options[i].text << endl;
            }

            int choice;
            cout << "> ";
            cin >> choice;
            
            // ... (Geri kalani ayni) ...
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
                // Item ismini manager'dan cekiyoruz (Gecici bir pointer ile)
                Item* temp = itemMgr.getItem(items[i].itemID);
                if (temp) {
                    cout << i + 1 << ". " << temp->getName() 
                         << " (" << items[i].price << " G)" << endl;
                    delete temp; // Sadece isim ogrenmek icin yarattik, siliyoruz
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
                // Secilen urunu al
                ShopItem selected = items[choice - 1];
                
                // Para yetiyor mu?
                if (hero.getGold() > selected.price) {
                    // Item'i gercekten olustur ve envantere ver
                    Item* newItem = itemMgr.getItem(selected.itemID);
                    if (hero.addItem(newItem)) {
                        hero.goldChange(-selected.price);
                        cout << ">>> PURCHASE SUCCESSFUL! <<<" << endl;
                    } else {
                        // Envanter doluysa parayi iade et ve itemi sil
                        cout << ">>> Inventory Full! Money refunded. <<<" << endl;
                        hero.goldChange(selected.price);
                        delete newItem;
                    }
                }
                else
                {
                    cout << "Not enough gold!";
                }
                
            } else {
                cout << "Invalid selection." << endl;
            }
        }
    }

    // --- ENVANTER MENUSU ---
    void manageInventory() {
        bool inMenu = true;
        while (inMenu) {
            cout << "\n--- INVENTORY ---" << endl;
            hero.printEquipment();
            hero.printInventory();
            cout << "Enter Item ID to Use/Equip (-1 to Exit): ";
            int inputIndex; cin >> inputIndex;
            if (inputIndex == -1) break;

            const auto& inv = hero.getInventory();
            if (inputIndex >= 0 && inputIndex < inv.size()) {
                Item* selected = inv[inputIndex];
                if (Weapon* w = dynamic_cast<Weapon*>(selected)) hero.equipWeapon(w);
                else if (Armor* a = dynamic_cast<Armor*>(selected)) hero.equipArmor(a);
                else if (Consumable* c = dynamic_cast<Consumable*>(selected)) hero.useItem(inputIndex);
            }
        }
    }

public:
Game() {
        cout << "Initializing Game Engine..." << endl;
        
        mapMgr.loadMap("Rooms.txt");
        currentRoom = mapMgr.getRoom(1); // Oyun 1. Odada baslar
        
        if (!currentRoom) cout << "CRITICAL ERROR: Room 1 not found!" << endl;

        // --- TEST 1: ODA 1'E MERHCANT EKLE (HEMEN TEST ETMEK ICIN) ---
        if (currentRoom) {
            // ID 51: Mysterious Merchant
            currentRoom->npcID.push_back(51); 
            cout << "DEBUG: Mysterious Merchant (ID 51) added to Room 1." << endl;
        }

        // --- TEST 2: ODA 4'E OLD SAGE EKLE (ESKI TEST DE DURSUN) ---
        Room* r4 = mapMgr.getRoom(4);
        if (r4) {
            // ID 50: Old Sage
            r4->npcID.push_back(50); 
            cout << "DEBUG: Old Sage (ID 50) added to Room 4 manually." << endl;
        }
    }

    void setupCheats() {
        Item* godSword = itemMgr.getItem(199);
        if (godSword) hero.addItem(godSword);
        cout << ">>> CHEAT MODE ON: God Sword added.\n" << endl;
    }

    void run() {
        if (!currentRoom) return;
        cout << "--- GAME START ---\n" << endl;
        bool isRunning = true;

        while (isRunning) {
            
            // 1. SAVAS KONTROLU
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

            // 2. NPC KONTROLU (Savas bittiyse veya yoksa calisir)
            if (!currentRoom->npcID.empty()) {
                // Ilk NPC'yi al
                int npcID = currentRoom->npcID[0];
                NPC* npc = npcMgr.getNPC(npcID);

                if (npc) {
                    // Eger ilk kez karsilasiyorsak OTOMATIK konus
                    if (!npc->hasMet()) {
                        cout << "\n>>> Someone approaches you..." << endl;
                        startDialogue(npc);
                    } else {
                        // Zaten tanisiyorsak bilgi ver
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
                    if (!currentRoom->npcID.empty()) {
                        NPC* npc = npcMgr.getNPC(currentRoom->npcID[0]);
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