#include <iostream>
#include "player.h"
#include "room.h"
#include "item.h"
#include "ItemManager.h"

int main(){

    Player hero;
    MapManager mapT;
    ItemManager itemT;
    Room* current_room;

    Item* wooden_sword = itemT.getItem(100);
    Item* potion = itemT.getItem(1);
    Item* armor = itemT.getItem(200);

    if (wooden_sword != nullptr) {
        hero.addItem(wooden_sword);
    }
    if (potion != nullptr) {
        hero.addItem(potion);
    }
    if (armor != nullptr)
    {
        hero.addItem(armor);
        hero.addItem(armor);
    }
    
    mapT.loadMap("Rooms.txt");
    current_room = mapT.getRoom(4);

    hero.printEquipment();
    hero.printInventory();
    armor->use(&hero);
    hero.printEquipment();
    hero.printInventory();
}