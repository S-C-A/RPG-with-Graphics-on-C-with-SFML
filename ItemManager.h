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
        // Format: ID, Isim, Aciklama, FIYAT, Stats
        
        ItemStats potStats; 
        potStats.healVal = 20; // 10'du 20 yaptim ornekle uyumlu olsun diye
        // Fiyat: 20 Gold
        itemDB[1] = new Consumable(1, "Kucuk Iksir", "20 Can yeniler", 20, potStats);

        ItemStats poisonStats;
        poisonStats.dmgVal = 10;
        // Fiyat: 30 Gold
        itemDB[2] = new Consumable(2, "Zehir Sisesi", "Dikkat yakar!", 30, poisonStats);

        // --- WEAPONS (Silahlar) ---
        // Format: ID, Isim, Aciklama, FIYAT, Hasar
        
        // Fiyat: 50 Gold
        itemDB[100] = new Weapon(100, "Tahta Kilic", "Baslangic silahi", 50, 5);

        // Fiyat: 150 Gold
        itemDB[101] = new Weapon(101, "Demir Kilic", "Keskin ve agir", 150, 12);

        // Fiyat: 5000 Gold
        itemDB[199] = new Weapon(199, "Master Sword", "Bestowed by gods", 5000, 999);

        // --- ARMORS ---
        // Format: ID, Isim, Aciklama, FIYAT, Defans
        
        // Fiyat: 100 Gold
        itemDB[200] = new Armor(200, "Deri Zirh", "Hafif koruma", 100, 3);
        
        // --- KEY ITEMS ---
        // Format: ID, Isim, Aciklama, FIYAT
        
        // Fiyat: 0 (Satilamaz gibi dusunebiliriz ama degeri 0)
        itemDB[300] = new KeyItem(300, "Pasli Anahtar", "Eski bir kapiyi acar", 0);
    }

    Item* getItem(int id) {
        if (itemDB.find(id) != itemDB.end()) {
            return itemDB[id]->clone(); 
        }
        return nullptr;
    }
};