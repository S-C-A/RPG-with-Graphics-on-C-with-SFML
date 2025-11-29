#pragma once
#include "item.h"
#include <map>

class ItemManager {
private: 
    std::map<int, Item*> itemDB;

public:
    ItemManager() {
        initItems();
    }

    ~ItemManager() {
        for (auto const& [id, item] : itemDB) {
            delete item;
        }
        itemDB.clear();
    }

    void initItems() {
        // --- CONSUMABLES ---
        ItemStats potStats; 
        potStats.healVal = 10;
        itemDB[1] = new Consumable(1, "Kucuk Iksir", "20 Can yeniler", potStats);

        ItemStats poisonStats;
        poisonStats.dmgVal = 10;
        itemDB[2] = new Consumable(2, "Zehir Sisesi", "Dikkat yakar!", poisonStats);

        // --- WEAPONS (Silahlar) ---
        itemDB[100] = new Weapon(100, "Tahta Kilic", "Baslangic silahi", 5);

        itemDB[101] = new Weapon(101, "Demir Kilic", "Keskin ve agir", 12);

        // --- ARMORS ---
        itemDB[200] = new Armor(200, "Deri Zirh", "Hafif koruma", 3);
        
        // --- KEY ITEMS ---
        itemDB[300] = new KeyItem(300, "Pasli Anahtar", "Eski bir kapiyi acar");
    }

    Item* getItem(int id) {
        if (itemDB.find(id) != itemDB.end()) {
            return itemDB[id];
        }
        return nullptr;
    }

};