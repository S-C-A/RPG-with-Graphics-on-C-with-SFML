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
    Item* leather_armor = itemT.getItem(200);
    Item* poison = itemT.getItem(2);
    Item* key = itemT.getItem(300);

    hero.addItem(potion);
    hero.addItem(leather_armor);
    hero.addItem(wooden_sword);
    hero.addItem(poison);
    hero.addItem(key);
    
    hero.printInventory();
    hero.printEquipment();
    hero.hurt(25);
    hero.useItem(4);
    hero.printInventory();
    hero.printEquipment();

}