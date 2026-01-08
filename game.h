#pragma once
#include <iostream>
#include <vector>

#include "player.h"
#include "ItemManager.h"
#include "enemyManager.h"
#include "room.h"

using namespace std;

class Game {
private:
    Player hero;
    ItemManager itemMgr;
    EnemyManager mobMgr;
    MapManager mapMgr;

    Room* currentRoom;

public:
    Game() {
        cout << "Starting the game..." << endl;
        // Dosya isminin dogru oldugundan emin ol
        mapMgr.loadMap("Rooms.txt");

        // Baslangic odasi ID: 1
        currentRoom = mapMgr.getRoom(1);
        
        if (currentRoom == nullptr) {
            cout << "HATA: Baslangic odasi bulunamadi! (ID: 1)" << endl;
        }
    }

    // --- OYUN DONGUSU (ARTIK BOS DEGIL) ---
    void run() {
        if (currentRoom == nullptr) return;

        cout << "--- OYUN BASLADI ---\n" << endl;
        bool isRunning = true;

        while (isRunning) {
            // 1. ODA BILGISINI GOSTER
            cout << "\n--------------------------------" << endl;
            cout << "KONUM: " << currentRoom->info << endl;
            
            // A. Yerde Esya Var mi?
            if (currentRoom->itemID != -1) {
                Item* tempItem = itemMgr.getItem(currentRoom->itemID);
                if (tempItem) {
                    cout << "YERDE: [" << tempItem->getName() << "] duruyor." << endl;
                    delete tempItem; // Sadece ismine baktik, hafizada yer kaplamasin.
                }
            }

            // B. Odada Canavar Var mi?
            if (currentRoom->monsterID != -1) {
                Monster* tempMob = mobMgr.getEnemy(currentRoom->monsterID);
                if (tempMob) {
                    cout << "DUSMAN: [" << tempMob->getName() << "] sana bakiyor!" << endl;
                    delete tempMob; // Sadece ismine baktik.
                }
            }
            cout << "--------------------------------" << endl;

            // 2. KOMUT AL
            cout << "Ne yapacaksin? (w,a,s,d: Yuru | i: Canta | e: Etkilesim | q: Cikis): ";
            char input;
            cin >> input;

            // 3. CANAVAR VARSA HAREKETI ENGELLE
            // Eger oyuncu yurumeye calisiyorsa ve odada canavar varsa:
            bool tryingToMove = (input == 'w' || input == 'a' || input == 's' || input == 'd');
            if (tryingToMove && currentRoom->monsterID != -1) {
                cout << ">>> KACAMAZSIN! Dusman yolunu kesti. Once savasmalisin (e)!" << endl;
                continue; 
            }

            // 4. KOMUTU ISLE
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
                    cout << "Oyundan cikiliyor..." << endl;
                    isRunning = false;
                    break;

                default: cout << "Gecersiz komut." << endl;
            }
        }
    }

    // --- YARDIMCI FONKSIYONLAR ---

    // Hareket Mantigi
    void move(int roomID) {
        if (roomID == -1) {
            cout << "O yonde gidemezsin (Duvar)." << endl;
        } else {
            Room* nextRoom = mapMgr.getRoom(roomID);
            if (nextRoom != nullptr) {
                currentRoom = nextRoom;
                cout << "Yuruyorsun..." << endl;
            } else {
                cout << "HATA: Gidilmek istenen oda (ID: " << roomID << ") bulunamadi!" << endl;
            }
        }
    }

    // Etkilesim (Savas / Loot) Mantigi
    void interactWithRoom() {
        // 1. DUSMAN VARSA -> SAVAS BASLAT
        if (currentRoom->monsterID != -1) {
            Monster* enemy = mobMgr.getEnemy(currentRoom->monsterID);
            
            if (enemy != nullptr) {
                cout << "\n>>> " << enemy->getName() << " ILE SAVAS BASLIYOR! <<<" << endl;
                
                // BURAYA ILERIDE COMBAT LOOP GELECEK
                // Simdilik otomatik kazaniyoruz:
                
                cout << "(Pat kut cat! Dusmani yendin...)" << endl;
                
                // Ganimet topla
                processLoot(enemy); 
                
                // ODADAN SILMEK COK ONEMLI:
                currentRoom->monsterID = -1; 
                
                delete enemy; // Kopyayi sil
            }
            return; 
        }

        // 2. ESYA VARSA -> AL
        if (currentRoom->itemID != -1) {
            Item* item = itemMgr.getItem(currentRoom->itemID);
            if (item) {
                cout << ">> " << item->getName() << " almaya calisiyorsun..." << endl;
                
                if (hero.addItem(item)) {
                    // Basarili alindiysa yerden sil
                    currentRoom->itemID = -1; 
                } else {
                    // Canta doluysa processLoot mantigini buraya da ekleyebiliriz
                    // Ama simdilik silip geciyoruz (Veya yerde birakabiliriz)
                    cout << "Canta dolu, esyayi alamadin." << endl;
                    delete item; 
                }
            }
        } else {
            cout << "Burada etkilesime gececek bir sey yok." << endl;
        }
    }

    // --- SENIN YAZDIGIN LOOT SISTEMI (Aynen korundu) ---
    void processLoot(Monster* mob) {
        if (mob->getGold() > 0) {
            hero.goldChange(mob->getGold());
            cout << "Ganimet: " << mob->getGold() << " altin kazandin!" << endl;
        }

        const vector<int>& drops = mob->getLootList();

        for (int itemID : drops) {
            Item* newItem = itemMgr.getItem(itemID);
            
            if (newItem == nullptr) continue;

            cout << ">> Dusen Esya: " << newItem->getName() << endl;

            if (hero.addItem(newItem)) {
                continue; 
            }

            char choice;
            cout << "Cantan dolu! " << newItem->getName() << " icin yer acmak ister misin? (y/n): ";
            cin >> choice;

            if (choice == 'y' || choice == 'Y') {
                bool itemDeleted = false;
                
                while (!itemDeleted) {
                    hero.printInventory();
                    
                    int slotToDelete;
                    cout << "Atilacak esyanin numarasi (Vazgecmek icin -1): ";
                    cin >> slotToDelete;

                    if (slotToDelete == -1) {
                        cout << newItem->getName() << " yerde birakildi." << std::endl;
                        delete newItem; 
                        break; 
                    }

                    if (hero.removeItem(slotToDelete)) {
                        itemDeleted = true; 
                        hero.addItem(newItem); 
                        cout << "Takas basarili!" << endl;
                    }
                    else {
                        cout << "Baska bir esya secin." << endl;
                    }
                }
            } 
            else {
                cout << newItem->getName() << " yerde birakildi." << endl;
                delete newItem;
            }
        }
    }
};