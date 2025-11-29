#include <iostream>
#include "player.h"
#include "room.h"
#include "item.h"
#include "ItemManager.h"
#include "condition.h"

int main(){

    Player hero;
    MapManager mapT;
    ItemManager itemT;
    Room* current_room;

    Item* wooden_sword = itemT.getItem(100);
    Item* potion = itemT.getItem(1);
    Item* leather_armor = itemT.getItem(200);
    Item* poison = itemT.getItem(2);
    Item* key = itemT.getItem(300);

std::cout << "\n--- TEST 2: GUC IKSIRI (BUFF) SENARYOSU ---" << std::endl;

    // 1. Normal Saldiri Gucu
    std::cout << "Normal Saldiri Gucu: " << hero.getAtk() << std::endl;

    // 2. Buff Efekti Olustur (Tur: STR_BUFF, Guc: 50, Sure: 2 Tur)
    StatusEffect gucIksiri(STR_BUFF, 50, 2);
    hero.addStatus(gucIksiri);
    std::cout << ">> Guc iksiri icildi! (+50 Hasar)" << std::endl;

    // 3. Buffli Saldiri Gucu (Hemen artmis olmali)
    std::cout << "Buffli Saldiri Gucu: " << hero.getAtk() << std::endl;

    // 4. TUR SIMULASYONU
    std::cout << "\n[TUR 1 GECTI]" << std::endl;
    hero.updateStatus(); // Sure 2 -> 1 oldu. Buff hala aktif.
    std::cout << "Hala Guclu mu?: " << hero.getAtk() << std::endl;

    std::cout << "\n[TUR 2 GECTI]" << std::endl;
    hero.updateStatus(); // Sure 1 -> 0 oldu. Buff silindi!
    
    std::cout << "\n[TUR 3 BASI]" << std::endl;
    std::cout << "Etki bitti, Guc: " << hero.getAtk() << std::endl;
    

}